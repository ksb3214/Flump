#!/usr/bin/perl -w

use POSIX ":sys_wait_h";
use Shout;
use FileHandle;
use IPC::Open2;
use MP3::Info;
use DBI;

$path = "/home/mp3";
$bps = 48;

$dbh = DBI->connect("DBI:mysql:flump:localhost", "flump", "flump") or die "Could not connect to DB";

my $con = new Shout(
##	ip		=> '195.112.36.254',
	ip => '127.0.0.1',
###ip		=> '192.168.0.10',
	port		=> 8000,
	mount		=> 'flump',
	password	=> 'flumpcode',
	icy_compat	=> 1,
#	aim		=> '',
#	icq		=> '',
##	irc 		=> '',
#	dumpfile	=> undef,	
	name		=> "Flump Radio @ $bps kbps",
	url		=> 'http://flump.amber.org.uk',
	format          => SHOUT_FORMAT_MP3,
	protocol        => SHOUT_PROTOCOL_ICY,
	genre		=> 'Mix',
	description	=> "Flump Radio @ $bps kbps",
	bitrate		=> $bps,
	ispublic	=> 0) or die "Could not construct shoutcast object";


$con->connect or die("Server dead: ",$con->error);

print "ok\n";

#print "Connecting...\n";
## try to connect
#if ($con->open) {
#    print "connected...\n";
#}
#else
#{
#print "failed to connect\n";
#	exit -1;
#}
#
#print "Done\n";

#         my $con = new Shout;
#
#         $con->host('127.0.0.1');
#         $con->port(8000);
#         $con->mount('/ices');
#         $con->password('flumpcode');
#         $con->dumpfile(undef);
#         $con->name('Flump Radio');
#         $con->url('http://flump.amber.org.uk');
#         $con->genre('Mix');
#         $con->format(SHOUT_FORMAT_MP3);
#         $con->protocol(SHOUT_PROTOCOL_ICY);
#         $con->description('Flump Radio');
#         $con->public(0);
#
#         ### Set your stream audio parameters for YP if you want
##         $con->set_audio_info(SHOUT_AI_BITRATE => 48, SHOUT_AI_SAMPLERATE => 28800);
#
#         ### Connect to the server
#         $con->open or die "Failed to open: ", $con->get_error;

exit(0);

print "here 1\n";
while(1==1)
{

# Get next song queue
$song="";

$sth = $dbh->prepare("select queued,filename,queue.id from mp3,queue where mp3.id=queue.id order by queued limit 0,1;");
$r = $sth->execute();
if(@row = $sth->fetchrow_array())
{
	$sth2 = $dbh->prepare("delete from queue where queued='" . $row[0] . "' and id='" . $row[2] . "';");
	$r = $sth2->execute();
	$sth3 = $dbh->prepare("update mp3 set played=played+1 where id=" . $row[2] . ";");
	$r = $sth3->execute();
	$song = "$path/" . $row[1];

}
$|=1;
if($song eq "")
{
	if(open(G,"</tmp/genre"))
	{
		$genre = <G>;
		$genre =~ s/[\r\n]//g;
		close(G);
		$sth = $dbh->prepare("select filename,id from mp3 where hide!=1 and genre='$genre' order by rand() limit 0,1;");
	} else {
		$sth = $dbh->prepare("select filename,id from mp3 where hide!=1 order by rand() limit 0,1;");
	}
	$r = $sth->execute();
	@row = $sth->fetchrow_array();
	$s = "$path/" . $row[0];
	print "R: ";
	play($s);
} else {
	# play track
	print "Q: ";
	play($song);
}

}

sub play
{
	my $bytes;
	$file = shift;
	print ">>> $file <<<\n";
	my $tag = get_mp3tag($file);
	$file =~ s/\(/\\\(/g;
	$file =~ s/\)/\\\)/g;
	$file =~ s/\'/\\\'/g;
	$file =~ s/ /\\ /g;
	$file =~ s/\&/\\\&/g;
	$finfo = $tag->{ARTIST} . " - " . $tag->{TITLE};
	open(P,">/tmp/playing");
	print P $finfo . "\n";
	close(P);
	
	print "$finfo\n";
	$con->updateMetadata(":# $finfo");
$|=1;

	if(open(STREAM,"lame --quiet -k -q 0 --mp3input -b $bps -m m $file - |"))
	{
		while( ($bytes = sysread( STREAM, $buffer, 4096 )) > 0 ) {
			print "-";
			$con->sendData($buffer) && next;
			print STDERR "Error while sending: ", $con->error, "\n";
			last;
		} continue {
			print ".";
			$con->sleep;
		}
		close(STREAM);
	}

}
