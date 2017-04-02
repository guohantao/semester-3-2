use 学生选课
go
alter database 学生选课
add filegroup 次要3
go
alter database 学生选课
add file
(
name = 次要3,
filename = 'c:\test\次要3.ndf',
size = 3mb,
maxsize = 20mb,
filegrowth = 10%
)
to filegroup 次要3