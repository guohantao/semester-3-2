with student_select_this_class ( student_id , student_name , class_id,grade) --找出选了这节课的学生集合
as (
select student.student_id , student.student_name , student.class_id,grade
from student,student_course
where student.student_id = student_course.student_id and
	  student_course.school_year = '2001/2002' and
	  student_course.course_id  in (select course.course_id    --找出严为这节课的course_id
									from teacher , teacher_course_class,course
									where teacher.teacher_id = teacher_course_class.teacher_id and
								       teacher.teacher_name = '严为' and
									   teacher_course_class.school_year = '2001/2002' and
									   course.course_id = teacher_course_class.course_id and
									   course.course_name = '软件开发技术')
),
max_grade (grade) as (select max(grade)   --从选了这堂课的学生中，找出最高分
		   from student_select_this_class)

select student_id , student_name , class_id, student_select_this_class.grade
from student_select_this_class,max_grade
where student_select_this_class.grade = max_grade.grade
					  



