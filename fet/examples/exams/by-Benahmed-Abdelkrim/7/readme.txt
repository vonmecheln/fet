Example contributed by Benahmed Abdelkrim, on 6 May 2026.

https://lalescu.ro/liviu/fet/forum/index.php?topic=6811.msg38209#msg38209

English:

In this example, the repeat of exam rooms for all teachers except the lead guardians on a given exam day was avoided. This differs from previous methods that failed to prevent repetition due to the use of three guarding subjects instead of one, each with its own independent set of preferred rooms. This required assigning a value greater than 1 to the constraint: A room's max activities per teacher. This approach is based on the following key elements:

- Using a single subject for all guarding activities, called for example: guarding

- Using a single subject for all reserve activities, called for example: Reserve

- Using three (3) activity tags: lead guardian 1, lead guardian 2, and lead guardian 3, corresponding to the number of exam days.  In this example, three activity tags are used because there are three days of exam.

Time Constraints:

- Add three (3) identical time constraints: A set of Activities has a set of preferred starting times for each of the three activity tags mentioned above.

Space Constraints:

a- Preferred rooms for a subject: Add two constraints for each of the following:

           - guarding subject: Select all rooms except reserve rooms.

           - Reserve subject: Select all the reserve rooms.

b- Preferred rooms for an activity tag: Add three (3) of these constraints for each of the three activity tags and select the real rooms: R1, R2, R3, ..., R14

c- room max activities per teacher: Add a constraint for each room. For real rooms max activities=3, while virtual rooms, max activities=1.

d- A set of activities occupies max different rooms for each of the activity tags mentioned above: Max different rooms=1.

---

Arabic:

في هذا المثال تم اجتناب تكرار القاعات لكل المدرسين باستثناء الحراس الرئيسين في يوم معين من أيام الامتحان، وهذا بخلاف الطرق السابقة التي فشلت في اجتناب التكرار بسبب استخدام 3 مواد للحراسة عوض مادة واحدة ولكل مادة مجموعة مستقلة من القاعات المفضلة، مما اضطرنا إلى اعطاء قيمة أكبر من 1 للقيد: أقصى أنشطة في القاعة لكل مدرس. تعتمد هذه الفكرة على العناصر الأساسية التالية:
  - استخدام مادة وحيدة لكل أنشطة الحراسة، تسمى على سبيل المثال: حراسة،
  - استخدام مادة وحيدة لكل أنشطة الاحتياط، تسمى على سبيل المثال: احتياط،
  - استخدام ثلاثة (3) وسوم تسمى: حارس رئيسي1، حارس رئيسي2، حارس رئيسي3، بحسب عدد أيام الاختبارات. في هذا المثال استخدم 3 وسوم لأن عدد أيام الاختبارات هي 3.

القيود الزمنية:
- اضافة ثلاثة(3) قيود زمنية متماثلة: أنشطة لها أوقات بدء مفضلة لكل وسم من الوسوم الثلاث المذكورة أعلاه

القيود المكانية:
أ- قاعات مفضلة لمادة: اضافة قيدين لكل من:
    - مادة الحراسة: نختار كل القاعات باستثناء قاعات الاحتياط،
    - مادة الاحتياط: نختار القاعات الخاصة بالاحتياط.

ب- قاعات مفضلة لوسم النشاط: اضافة ثلاثة (3) قيود من هذا النوع لكل وسم من الوسوم الثلاث ونختار القاعات الحقيقية
R1, R2, R3, ...,R14

ج- أقصى أنشطة في القاعة: اضافة قيد لكل قاعة، القاعات الحقيقية لها أقصى أنشطة=3، القاعات الوهمية لها أقصى أنشطة=1

د- أنشطة تحتل أقصى قاعات مختلفة لكل وسم من الوسوم المذكورة أعلاه: أقصى قاعات مختلفة=1
