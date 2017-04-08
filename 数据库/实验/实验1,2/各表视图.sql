create view book_view as
select book_name,publish_company,author,price
from book$

go
create view class_view as
select class_id,monitor,department_id
from class$

go
create view class_course_view as
select class_id,course_id
from class_course$
go
create view course_view as
select course_id,course_name,total_perior,week_perior,credit
from course$
go
create view department_view as
select department_name,department_header,teacher_num
from deparment$
go
create view student_view as
select student_id,student_name,sex,birth,class_id,entrance_date
from student$
go
create view student_corse_view as
select course_id,student_id,credit,semester,school_year
from student_course$
go
create view teacher_view as
select teacher_id,teacher_name,sex,birth,department_id,profession,telephone
from teacher$
go
create view teacher_course_class_view as
select teacher_id,course_id,class_id,semester,school_year,course_classroom,book_id
from teacher_course_class$
