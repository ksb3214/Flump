DROP TABLE IF EXISTS attributes;
CREATE TABLE attributes (
	object int(12) unsigned,
	name varchar(30),
	value longblob
) TYPE=MyISAM;

