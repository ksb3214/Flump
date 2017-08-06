DROP TABLE IF EXISTS bugs;
CREATE TABLE bugs (
  user int(10) default NULL,
  seq int(3) default NULL,
  status char(1) default NULL,
  posted timestamp NOT NULL,
  class char(1) default NULL,
  ticket int(10) default NULL,
  room varchar(30) default NULL,
  comment text
) TYPE=MyISAM;

