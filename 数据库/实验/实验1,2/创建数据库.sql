create database 学生选课
on primary
(
name = 学生选课,
filename = 'c:\test\学生选课.mdf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
),

filegroup 次要文件组1
(
name = 次要1,
filename = 'c:\test\次要文件1.ndf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
),

filegroup 次要文件组2
(
name = 次要2,
filename = 'c:\test\次要文件2.ndf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
)

log on
(
name = 学生选课_log,
filename = 'c:\test\学生选课_log.ldf',
size = 1MB,
maxsize = 20MB,
filegrowth = 10%
)