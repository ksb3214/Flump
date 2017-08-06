-- MySQL dump 10.8
--
-- Host: localhost    Database: flumpksb
-- ------------------------------------------------------
-- Server version	4.1.7

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE="NO_AUTO_VALUE_ON_ZERO" */;

--
-- Table structure for table `attributes`
--

DROP TABLE IF EXISTS `attributes`;
CREATE TABLE `attributes` (
  `object` int(12) unsigned default NULL,
  `name` varchar(30) default NULL,
  `value` longblob
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `banname`
--

DROP TABLE IF EXISTS `banname`;
CREATE TABLE `banname` (
  `name` varchar(64) default NULL,
  `who` int(10) default '1',
  `added` datetime default NULL,
  KEY `nnidx` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `bugs`
--

DROP TABLE IF EXISTS `bugs`;
CREATE TABLE `bugs` (
  `user` int(10) default NULL,
  `seq` int(3) default NULL,
  `status` char(1) default NULL,
  `posted` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `class` char(1) default NULL,
  `ticket` int(10) default NULL,
  `room` varchar(30) default NULL,
  `comment` text
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `help`
--

DROP TABLE IF EXISTS `help`;
CREATE TABLE `help` (
  `id` bigint(20) NOT NULL auto_increment,
  `user` int(11) default NULL,
  `posted` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `entry` text,
  `title` text,
  `aliases` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `motd`
--

DROP TABLE IF EXISTS `motd`;
CREATE TABLE `motd` (
  `id` int(5) unsigned NOT NULL auto_increment,
  `message` blob,
  `recorded` bigint(14) default NULL,
  `who` bigint(9) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `mp3`
--

DROP TABLE IF EXISTS `mp3`;
CREATE TABLE `mp3` (
  `id` int(10) unsigned default NULL,
  `artist` varchar(255) default NULL,
  `title` varchar(255) default NULL,
  `album` varchar(255) default NULL,
  `year` int(4) unsigned default NULL,
  `genre` varchar(255) default NULL,
  `filename` varchar(255) default NULL,
  `md5` varchar(255) default NULL,
  `hide` int(1) default '0',
  `added` datetime default NULL,
  `played` int(10) unsigned default '0',
  `min` int(5) default NULL,
  `sec` int(5) default NULL,
  KEY `fnidx` (`filename`),
  KEY `genidx` (`genre`),
  KEY `md5idx` (`md5`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `news`
--

DROP TABLE IF EXISTS `news`;
CREATE TABLE `news` (
  `id` int(5) unsigned NOT NULL auto_increment,
  `message` blob,
  `recorded` bigint(14) default NULL,
  `who` bigint(9) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `object`
--

DROP TABLE IF EXISTS `object`;
CREATE TABLE `object` (
  `id` int(12) unsigned NOT NULL auto_increment,
  `type` int(12) unsigned default NULL,
  `name` varchar(45) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `objects`
--

DROP TABLE IF EXISTS `objects`;
CREATE TABLE `objects` (
  `id` int(12) unsigned NOT NULL auto_increment,
  `type` int(12) unsigned default NULL,
  `name` varchar(45) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `prefer`
--

DROP TABLE IF EXISTS `prefer`;
CREATE TABLE `prefer` (
  `uid` bigint(9) default NULL,
  `prefuid` bigint(9) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `queue`
--

DROP TABLE IF EXISTS `queue`;
CREATE TABLE `queue` (
  `id` int(10) unsigned default NULL,
  `user` int(10) unsigned default NULL,
  `queued` bigint(14) NOT NULL default '0',
  `shown` int(3) unsigned default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `quotes`
--

DROP TABLE IF EXISTS `quotes`;
CREATE TABLE `quotes` (
  `id` bigint(20) NOT NULL auto_increment,
  `user` int(11) default NULL,
  `posted` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `quote` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `slap`
--

DROP TABLE IF EXISTS `slap`;
CREATE TABLE `slap` (
  `slap` varchar(50) default NULL,
  `id` int(10) NOT NULL auto_increment,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `smess`
--

DROP TABLE IF EXISTS `smess`;
CREATE TABLE `smess` (
  `id` int(5) unsigned NOT NULL auto_increment,
  `ufrom` bigint(9) default NULL,
  `uto` bigint(9) default NULL,
  `message` blob,
  `recorded` bigint(14) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

