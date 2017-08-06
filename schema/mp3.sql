-- MySQL dump 9.09
--
-- Host: localhost    Database: flump
-- ------------------------------------------------------
-- Server version	4.0.16-log

--
-- Table structure for table `mp3`
--

CREATE TABLE mp3 (
  id int(10) unsigned default NULL,
  artist varchar(255) default NULL,
  title varchar(255) default NULL,
  album varchar(255) default NULL,
  year int(4) unsigned default NULL,
  genre varchar(255) default NULL,
  filename varchar(255) default NULL,
  md5 varchar(255) default NULL,
  hide int(1) default '0',
  added datetime default NULL,
  played int(10) unsigned default '0',
  KEY fnidx (filename),
  KEY genidx (genre)
) TYPE=MyISAM;

