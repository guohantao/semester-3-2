use ѧ��ѡ��
go
alter database ѧ��ѡ��
add filegroup ��Ҫ3
go
alter database ѧ��ѡ��
add file
(
name = ��Ҫ3,
filename = 'c:\test\��Ҫ3.ndf',
size = 3mb,
maxsize = 20mb,
filegrowth = 10%
)
to filegroup ��Ҫ3