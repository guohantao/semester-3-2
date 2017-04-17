select student.student_id
from student inner join student_course on student.student_id = student_course.student_id
	inner join course on course.course_id = student_course.course_id
where course.course_name = 'µç´Å²¨¹¤³Ì'