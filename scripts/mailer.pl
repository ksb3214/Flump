#!/usr/bin/perl -w

use Mail::Sendmail;
use DBI;

# Settings

$db_host = "localhost";
$db_user = "flump";
$db_password = "flump";
$db_name = "flump";
$mail_host = "utonium.root-access.ws";
$mail_from = "flump\@amber.org.uk";

# End of settings

$file = shift;
$subject = shift;

if($subject eq "")
{
	print "Usage: $0 <message file> <subject>\n";
	exit(10);
}

if(!open(F,"<$file"))
{
	print "$0: Unable to open file $file.\n";
	exit(10);
}

while(not(eof(F)))
{
	$message .= <F>;
}
close(F);

print "\n";
print "Sending message from $file with subject:\n";
print "$subject\n\n";

$ans="";
while ($ans ne "y" and $ans ne "n")
{
	print "Do you want to send this message now? (Yes/No/View) : ";
	$|=1;
	$ans = <STDIN>;
	chop($ans);
	if($ans eq "v")
	{
		print "================ Message begins ==================\n";
		print $message;
		print "================= Message ends ===================\n";
	}
}

if($ans eq "n")
{
	print "Message sending aborted.\n";
	exit(0);
}

	$dbh = DBI->connect("DBI:mysql:$db_name:$db_host",$db_user,$db_password);
	$sth = $dbh->prepare("select value from attributes where name='email' and value like '%@%.%';");
	$rv=0;
	$rv = $sth->execute;
	while(@row = $sth->fetchrow_array)
	{
		print $row[0] . "...";
		$|=1;
		%mail = ( To      => $row[0],
                   From    => $mail_from,
                   Message => $message,
		   Subject => $subject,
	           smtp	   => $mail_host
		);

		if(sendmail(%mail))
		{
			print "ok\n";
		} else {
			print "fail\n";
			print $Mail::Sendmail::error;
		}
	}
	print "All mails sent.\n";
