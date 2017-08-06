-- MySQL dump 9.09
--
-- Host: localhost    Database: flump
-- ------------------------------------------------------
-- Server version	4.0.16-log

--
-- Table structure for table `help`
--

DROP TABLE IF EXISTS help;
CREATE TABLE help (
  id bigint(20) NOT NULL auto_increment,
  user int(11) default NULL,
  posted timestamp(14) NOT NULL,
  entry text,
  title text,
  aliases text,
  PRIMARY KEY  (id)
) TYPE=MyISAM;

