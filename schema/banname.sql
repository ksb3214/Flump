-- MySQL dump 9.09
--
-- Host: localhost    Database: flump
-- ------------------------------------------------------
-- Server version	4.0.16-log

--
-- Table structure for table `banname`
--

CREATE TABLE banname (
  name varchar(64) default NULL,
  who int(10) default '1',
  added datetime default NULL,
  KEY nnidx (name)
) TYPE=MyISAM;

