#!/usr/bin/perl

use DBI;

$dbh = DBI->connect("DBI:mysql:flump:localhost", "flump", "flump123");

$sth = $dbh->prepare("select md5,count(md5) from mp3 where hide!=1 group by md5 order by md5;");
$r = $sth->execute();
while(@row = $sth->fetchrow_array())
{
	if($row[1]>1)
	{
		print $row[0] . " - " . $row[1] . "\n";
		$sth2 = $dbh->prepare("select id from mp3 where md5='" . $row[0] . "' and hide!=1 limit 0,1;");
		$r = $sth2->execute();
		@row2 = $sth2->fetchrow_array();
		$sth2 = $dbh->prepare("update mp3 set hide=1 where id='" . $row2[0] . "';");
		$r = $sth2->execute();
	}
}
