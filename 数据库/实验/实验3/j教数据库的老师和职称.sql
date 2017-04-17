select teacher.teacher_name,teacher.profession
from teacher inner join teacher_course_class on teacher.teacher_id = teacher_course_class.teacher_id
	 inner join course on course.course_id = teacher_course_class.course_id
where course.course_name = 'SQL Server数据库开发技术'