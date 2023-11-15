[toc]
# Sqlile

## 介绍 

### 特性

1. 零配置
2. 可移植
3. 紧凑
4. 简单
5. 灵活
6. 自由的授权
7. 可靠
8. 易用

### 数据类型

一般数据采用固定的静态数据类型，而SQLite采用的是鼎泰数据类型，会根据存入值自定判断
- SQLite具有以下5中基本数据类型
  
    1. integer： 带符号的整型（最多64位）
    2. real： 8字节表示的浮点型
    3. text： 字符类型，支持多种编码（如UTF-8，UTF-16），无大小限制
    4. blob：任意类型的数据，无大小限制。BLOB（binary large object）二进制对象，使用二进制保存数据
    5. null 表示空

### Sql语句

>### 创建打开sqlite数据库

```shell
sqlite *.db 
# 当*.db不存在时，则创建并打开数据库文件
# 当*.db存在时，则打开数据库文件 
```
>### 退出数据库

```shell
.quit 或 .exit
```

>### sql的语句格式
```
所有的SQL语句都是以分号结尾的，SQL语句不区分大小写。两个减号“--”代表注释
关系型数据库的核心操作
    1. 创建，修改，删除表
    2. 添加，修改，删除行
    3. 查表
```

>### 创建表: create语句
```sql
create table <tablename>(<字段> <类型>,<字段> <类型>...);
#example： 
create table tab(id integer,name text);

# 查看当前数据库的表
.table
# 查看当前数据库的表的创建语句
.schema
```
![查看表](./Sqlite.assets/sqlitelooktab.png)

>### 设置主键

在用sqlite设计表时，每个表都可以通过primary key手动设置主键，每个表只能有一个主键，设置为主键的列数据不能重复。
```sql
create table <tablename>(<字段> <类型> primary key,<字段> <类型>...);
#example： 
create table tab(id integer primary key,name text);
```

>### 修改表：alter 语句

在已有的表中添加或删除字段以及修改表名，无法修改字段属性
```sql
#修改表名
alter table <tablename> rename to <newname>;
#example
alter table stu2 rename to stu;
```
```sql
#添加字段
alter table <tablename> add <字段> <属性>;
#example 该例添加了字段 grade 属性为 integer
alter table stu add grade integer;
```

>### 删除表 drop语句

用于删除表（表的结构，属性，以及表的索引也会被删除）
```sql
#删除表
drop table <表名>;
#example 删除表
drop table nstu;
```

>### 插入新行 insert into语句

给一行中所有列赋值
```sql
# 插入新行 当列值为字符串时，要加上单引号 ''
insert into <表名> values (<列值1>,<列值2>,<列值3>,<列值4>,...);
# example 
insert into tbl values(100,'朱芷杰' ,'ahhs');
```
给一行中指定列赋值
```sql
# 插入新行 当列值为字符串时，要加上单引号 ''
insert into <tablename>(<字段1>,<字段2>...) values (<列值1>,<列值2>,...);
# example 
INSERT INTO stu(id, name ,grade) values (1, 'zzj', 99);
```

>### 修改行数据 update语句

修改行信息
```sql
# 修该满足条件行的字段的值
update <tablename> set <字段>=<值> where <条件>; 
# example  修改满足id=2的行中addr字段的值
update nstu set addr='cdu' where id=2;
```

>### 删除行 delete语句

适应where根据匹配条件，查找一行或多行，根据查找的结果删除表中查找到的行
```sql
-- 删除满足条件行的字段的值
delete from <tablename> where <匹配条件>；
# example  删除满足pass='123'的行
delete from newusr where pass='123';
```


>### 查询 select语句
用于从表中选取数据，结果被存储在一个结果表中（称为结果集）
```sql
select * from <tablename> <匹配条件>
select <字段1>,<字段2>,<字段3>... from <tablename> <匹配条件> 

```

where子句用于规定匹配的条件

 

|   运算符    | 描述                                                       |
| :---------: | ---------------------------------------------------------- |
|      =      | 等于                                                       |
|     <>      | 不等于。**注释：**在 SQL 的一些版本中，该操作符可被写成 != |
|      >      | 大于                                                       |
|      <      | 小于                                                       |
|     >=      | 大于等于                                                   |
|     <=      | 小于等于                                                   |
| between and | 在某个范围内                                               |
|    LIKE     | 模糊查找                                                   |
|     IN      | 指定针对某个列的多个可能值                                 |

>### 复制表以及修改表结构

复制表
```sql
-- 复制整张表
create table <newtable> as select * from <oldtable> 
-- 复制表的部分内容
create table <newtable> as select * from <oldtable> where <匹配条件>

```

修改表的结构
```sql
-- 第一步 创建新表
create table <newtable>(<字段> <类型>,<字段> <类型>...);
-- 第二步 导入数据
insert into <newtable>(<字段1>,<字段2>,<字段3>...) select <字段1>,<字段2>,<字段3> from <oldtable>;
-- 第三步 删除旧表
drop table <oldtable>;
-- 第四步 修改表名  将新创建的表名修改为旧的表名
alter table <newtable> rename to <oldtable>;
```


>## 事务

事务(Transaction)可以使用BEGINTRANSACTION命令或简单的BEGIN命令来启动。此类事务通常会持续执行下去，直到遇到下一个COMMIT或ROLLBACK命令。不过在数据库关闭或发生错误时，事务处理也会回滚。

SQLite中，默认情况下，每条sql语句自成事务
- begin  开始一个事务，之后所有的操作都可以取消
- commit 使begin后的所有命令的到确认
- rollback 取消begin后的所有命令。被commit后的命令除外

>## 函数



>## C编程

> 打开关闭数据库
```c
int sqlite3_open(char *db_name, sqlite3 **db);


int sqlite3_close(sqlite3 *db);

typedef int (*sqlite3_callback)(void *args, int f_num, char **f_value, char **f_name);
// args 传入回调的参数
// f_num 记录中字段数目
// f_value 包含每个字段的值指针数组
// f_name 字段名称的指针数组

int sqlite3_exec(sqlite3 *db, const char *sql, sqlite3_callback callback, void *, char **errmsg);
// 功能：执行SQL操作
// db：数据库句柄
// sql：SQL语句
// callback：回调函数
// errmsg：错误信息指针的地址
// 返回值：成功返回0，失败返回错误码

int sqlite3_get_table(sqlite3 *db, const char *sql, char ***resultp, int*nrow, int *ncolumn, char **errmsg);
// 功能：执行SQL操作
// db：数据库句柄
// sql：SQL语句
// resultp：用来指向sql执行结果的指针
// nrow：满足条件的记录的数目
// ncolumn：每条记录包含的字段数目
// errmsg：错误信息指针的地址
// 返回值：成功返回0，失败返回错误码
```

