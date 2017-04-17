create view student_select_view as
select student.student_id , student.student_name , department.department_name, teacher_name, course_name,book_name, publish_company, student_course.credit, student_course.grade
from student, class, teacher, teacher_course_class, student_course, book, course,department
where student.student_id = student_course.student_id and
	  student.class_id = class.class_id and
	  teacher.teacher_id = teacher_course_class.teacher_id and
	  course.course_id = student_course.course_id and
	  course.book_id = book.[book_id ] and
	  student_course.course_id = teacher_course_class.course_id and
	  department.department_id = class.department_id and
	  student_course.school_year = teacher_course_class.school_year