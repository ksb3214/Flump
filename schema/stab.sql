DROP TABLE IF EXISTS stab_items;
CREATE TABLE stab_items (
  item varchar(50) default NULL,
  id int(10) NOT NULL auto_increment,
  PRIMARY KEY  (id)
) TYPE=MyISAM CHARSET=latin1;

DROP TABLE IF EXISTS stab_locations;
CREATE TABLE stab_locations (
  location varchar(50) default NULL,
  id int(10) NOT NULL auto_increment,
  PRIMARY KEY  (id)
) TYPE=MyISAM CHARSET=latin1;


insert into stab_items values ("coat hanger",1);
insert into stab_items values ("nail file",2);
insert into stab_items values ("strawberry blancmange",3);
insert into stab_items values ("sledge hammer",4);
insert into stab_items values ("breast implant",5);


insert into stab_locations values("nose",1);
insert into stab_locations values("head",2);
insert into stab_locations values("eye",3);
insert into stab_locations values("nipple",4);
insert into stab_locations values("foot",5);
insert into stab_locations values("arse",6);
