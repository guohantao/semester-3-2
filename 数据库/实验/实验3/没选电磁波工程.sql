select student.student_id, student.student_name
from student 
where student.student_id not in (select student.student_id
								from student inner join student_course on student.student_id = student_course.student_id
								inner join course on course.course_id = student_course.course_id
								where course.course_name = '��Ų�����')
--����ֱ��д where course_name != ��Ų�  
-- ��Ϊ�п�����ѧ����ѡ�˵�Ų�����ѡ�������Σ�����ͬ���ᱻ�㵽����