
select student.student_id , student.student_name
from student,student_course
where student.student_id = student_course.student_id 
group by student.student_id,student_name
	having count(student_course.course_id) >=3
	  
	  


		