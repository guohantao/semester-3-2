select student.student_id, student.student_name
from student 
where student.student_id not in (select student.student_id
								from student inner join student_course on student.student_id = student_course.student_id
								inner join course on course.course_id = student_course.course_id
								where course.course_name = '电磁波工程')
--不能直接写 where course_name != 电磁波  
-- 因为有可能有学生即选了电磁波，又选了其他课，这样同样会被算到里面