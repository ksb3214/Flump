#!/usr/bin/perl

use DBI;
use MP3::Info;
use Digest::MD5 qw(md5_hex);

$dbh = DBI->connect("DBI:mysql:flump:localhost", "flump", "flump");

$path = shift;

print "Scanning...\n";

if(open(F,'find "' . $path . '" -name "*.mp3" -print|'))
{
while(not(eof(F)))
{
	$f = <F>;
	chop($f);
	$f =~ s/"//g;
#        if(open(MP,"<$f"))
#        {
#                $offset=0;
#                read MP,$h,3,0;
#                if($h eq "ID3")
#                {
#                        seek MP,6,SEEK_SET;
#                        read MP,$h,4;
#                        $b0 = ord(substr($h,3,1));
#                        $b1 = ord(substr($h,2,1));
#                        $b2 = ord(substr($h,1,1));
#                        $b3 = ord(substr($h,0,1));
#                        $offset = $b0 + ($b1*256) + ($b2*(256**2)) + ($b3*(256**3));
#                        $offset = $offset+10;
#                }
#                seek MP,$offset,SEEK_SET;
#                $tag = 0;
#                $data = "";
#                while(read(MP,$h,10000000)>0)
#                {
#                        $data = $data . $h;
#                }
#                close(MP);
#                @d = split(/TAG/,$data);
#                $data = @d[0];
#        }
#        $md5 = md5_hex($data);
#	$sth = $dbh->prepare("select id from mp3 where md5='$md5';");
#	$r = $sth->execute();
#	if(@row = $sth->fetchrow_array)
#	{
#	} else {
#		print "Copying $f...";
#		$|=1;
#		`cp "$f" .`;
#		print "Done.\n";
#		$|=1;
		if(($f) =~ /\/([^\/]*)$/)
		{
			$bf = $1;
		} else {
			$bf = $f;
		}
		print "Normalizing $bf...\n";
		$|=1;
		`mp3gain -c -q -r "$bf"`;
#		print "Done.\n";
		$|=1;
#	}
}
close(F);
}
print "Gather complete - you should now run the scan script.\n";
