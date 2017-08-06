#!/usr/bin/perl 

use DBI;
use MP3::Info;
use Digest::MD5 qw(md5_hex);

$dest = "/home/mp3";
$user = $ENV{UPLOAD_VUSER};
$genre = "uploaded";
$file = shift;

($file) =~ /\/([^\/]+).mp3$/i;
$base = $1;

$destfile = "$user/$base.mp3";

system qq{mkdir $dest/$user};


open(LOG,">>/home/mp3upload/uploads.log");

$dbh = DBI->connect("DBI:mysql:flump:localhost", "flump", "flump");

	$sth = $dbh->prepare(qq{select md5 from mp3 where filename="$destfile";});
	$r = $sth->execute();
	if(@row = $sth->fetchrow_array())
	{
		print LOG "Skipping $file (Already Stored)\n";
		unlink($file);
	} else {
		print LOG "Uploaded $file as $destfile\n";

		if(open(MP,"<$file"))
		{
			$offset=0;
			read MP,$h,3,0;
			if($h eq "ID3")
			{
				seek MP,6,SEEK_SET;
				read MP,$h,4;
				$b0 = ord(substr($h,3,1));
				$b1 = ord(substr($h,2,1));
				$b2 = ord(substr($h,1,1));
				$b3 = ord(substr($h,0,1));
				$offset = $b0 + ($b1*256) + ($b2*(256**2)) + ($b3*(256**3));
				$offset = $offset+10;
			}
			seek MP,$offset,SEEK_SET;
			$tag = 0;
			$data = "";
			while(read(MP,$h,10000000)>0)
			{
				$data = $data . $h;
			}
			close(MP);
			@d = split(/TAG/,$data);
			$data = @d[0];
		}
		$md5 = md5_hex($data);

		$sth2 = $dbh->prepare("select id from mp3 where md5='$md5';");
		$r = $sth2->execute();
		if(@row2 = $sth2->fetchrow_array())
		{
			print LOG "Skipping $file (Duplicate Checksum)\n";
			unlink($file);
		} else {
			my $tag = get_mp3tag($file);
			$info = get_mp3info($file);
			$min = $info->{MM};
			$sec = $info->{SS};

			$title = $tag->{TITLE};
			$artist = $tag->{ARTIST};
			$album = $tag->{ALBUM};
			$year = $tag->{YEAR};
			$title =~ s/"//g;
			$album =~ s/"//g;
			$artist =~ s/"//g;
			if (($artist ne "") and ($title ne "")) {
				system qq{mv "$file" "$dest/$destfile"};
				$sth2 = $dbh->prepare("select max(id) as mid from mp3;");
				$r = $sth2->execute();
				@row2 = $sth2->fetchrow_array();
				$id = $row2[0]+1;
				$sth = $dbh->prepare(qq{insert into mp3 set filename="$destfile",artist="$artist",title="$title",album="$album",year="$year",genre="$genre",md5="$md5",id=$id,added=now(),min=$min,sec=$sec;});
				$r = $sth->execute();
				print LOG "Adding $artist - $title ($album) [$min:$sec]\n";
				system qq{mp3gain -c -q -r "$dest/$destfile"};
			} else {
				print LOG "Skipping $file (No ID3 Data)\n";
				print LOG "$title - $artist - $album\n";
				unlink($file);
			}
		}
	}
close(LOG);
