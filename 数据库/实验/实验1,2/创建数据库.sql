create database ѧ��ѡ��
on primary
(
name = ѧ��ѡ��,
filename = 'c:\test\ѧ��ѡ��.mdf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
),

filegroup ��Ҫ�ļ���1
(
name = ��Ҫ1,
filename = 'c:\test\��Ҫ�ļ�1.ndf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
),

filegroup ��Ҫ�ļ���2
(
name = ��Ҫ2,
filename = 'c:\test\��Ҫ�ļ�2.ndf',
size = 3MB,
maxsize = 50MB,
filegrowth = 10%
)

log on
(
name = ѧ��ѡ��_log,
filename = 'c:\test\ѧ��ѡ��_log.ldf',
size = 1MB,
maxsize = 20MB,
filegrowth = 10%
)