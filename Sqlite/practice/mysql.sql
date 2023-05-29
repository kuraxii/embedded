-- 

-- 添加指定的字段
insert into tbl(id, addr) values (102, 'cdu');

-- 显示当前数据库中表的创建语句
.schema

-- 在表中添加字段
ALTER TABLE tbl ADD num text;

-- 添加指定的字段
INSERT INTO stu(id, name ,grade) values (1, 'zzj', 99);

-- 通过已有表创建表
create table nstu as select * from stu;

-- 查找表
SELECT * FROM nstu;

-- 删除表
drop table tbl;

.schema