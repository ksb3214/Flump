#!/bin/sh
/usr/local/bin/mysqldump -a --add-drop-table -uflump -pflump flump > /usr/home/flump/backup/`/bin/date +%Y%m%d`.sql
/usr/bin/gzip /usr/home/flump/backup/`/bin/date +%Y%m%d`.sql

