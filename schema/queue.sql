-- MySQL dump 8.23
--
-- Host: localhost    Database: mp3
---------------------------------------------------------
-- Server version	3.23.58

--
-- Table structure for table `queue`
--

DROP TABLE IF EXISTS queue;
CREATE TABLE queue (
  id int(10) unsigned default NULL,
  user int(10) unsigned default NULL,
  queued bigint(14) NOT NULL,
  shown int(3) default NULL
) TYPE=MyISAM;

