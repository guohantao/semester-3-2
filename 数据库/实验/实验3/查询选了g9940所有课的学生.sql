select student.student_id , student.student_name
from student 
where not exists ( (select course_id
					from student_course
					where student_course.student_id = 'g9940201' )
					except
					(select course_id
					 from student_course
					 where student.student_id = student_course.student_id))