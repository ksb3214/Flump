-- MySQL dump 9.09
--
-- Host: localhost    Database: flump
-- ------------------------------------------------------
-- Server version	4.0.16-log

--
-- Table structure for table `mp3`
--

DROP TABLE IF EXISTS smess;
CREATE TABLE smess (
  id int(5) unsigned auto_increment primary key,
  ufrom bigint(9) default NULL,
  uto bigint(9) default NULL,
  message blob default NULL,
  recorded bigint(14) default NULL
) TYPE=MyISAM;

