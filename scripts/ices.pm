#!/usr/bin/perl -w

use POSIX ":sys_wait_h";
use FileHandle;
use IPC::Open2;
use MP3::Info;
use DBI;

my $path;
my $dbh;

sub ices_init()
{
	$path = "/home/mp3";
	$dbh = DBI->connect("DBI:mysql:flump:localhost", "flump", "flump") or die "Could not connect to DB";
}

sub ices_get_next
{
	$song="";

	$sth = $dbh->prepare("select queued,filename,queue.id,artist,title from mp3,queue where mp3.id=queue.id order by queued limit 0,1;");
	$r = $sth->execute();
	if(@row = $sth->fetchrow_array())
	{
		$sth2 = $dbh->prepare("delete from queue where queued='" . $row[0] . "' and id='" . $row[2] . "';");
		$r = $sth2->execute();
		$sth3 = $dbh->prepare("update mp3 set played=played+1 where id=" . $row[2] . ";");
		$r = $sth3->execute();
		$song = "$path/" . $row[1];
		$finfo = $row[3] . " - " . $row[4];
		open(P,">/tmp/playing"); 
		print P $finfo . "\n";
		close(P);
	}
	if($song eq "")
	{
		if(open(G,"</tmp/genre"))
		{
			$genre = <G>;
			$genre =~ s/[\r\n]//g;
			close(G);
			$sth = $dbh->prepare("select filename,id,artist,title from mp3 where hide!=1 and genre='$genre' order by rand() limit 0,1;");
		} else {
			$sth = $dbh->prepare("select filename,id,artist,title from mp3 where hide!=1 order by rand() limit 0,1;");
		}
		$r = $sth->execute();
		@row = $sth->fetchrow_array();
		$s = "$path/" . $row[0];
		$finfo = $row[2] . " - " . $row[3];
		open(P,">/tmp/playing"); 
		print P $finfo . "\n";
		close(P);
		return($s);
	} else {
		return($song);
	}
}

1;
