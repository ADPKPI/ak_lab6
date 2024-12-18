## Архітектура комп'ютерів 2. Процесори  
### Лабораторна робота №6  

**Виконав:**  
студент групи ІО-23  
**Прохоренко А.Д.**  
---

У рамках лабораторної роботи було виконано модифікацію модуля ядра Linux із попередньої лабораторної роботи №4, зосередившись на розширенні функціональності та впровадженні додаткових механізмів перевірки й обробки помилок.

### Модифікації коду  

#### 1. Перевірка параметра за допомогою BUG_ON  
У функції `hello_init` було додано перевірку значення параметра `count` за допомогою макросу `BUG_ON`. Якщо параметр `count` перевищує значення `10`, система викликає критичну помилку (panic), що дозволяє зупинити виконання модуля та захистити його від некоректного використання.

Код перевірки виглядає так:
```c
BUG_ON(count > 10);
```

Якщо значення `count > 10`, у логах з’являється повідомлення:
```
kernel BUG at hello.c:27!
```

#### 2. Симуляція помилки виділення пам’яті

У модуль додано механізм симуляції ситуації, ніби функція kmalloc() повернула NULL. Це відбувається для останнього виклику функції друку, що дозволяє перевірити стійкість модуля до таких сценаріїв. У разі виникнення цієї ситуації також викликається BUG_ON.

Реалізація симуляції:
```c
struct time_data *data = kmalloc(sizeof(*data), GFP_KERNEL);

// Симуляція помилки для останнього виклику
if (!data && i == count - 1) {
    BUG_ON(!data); // Викликаємо BUG_ON
}
```

У логах з’являється помилка:
```
BUG: unable to handle kernel NULL pointer dereference
```

3. Друк часу виконання функції
4. 
У функції hello_exit реалізовано механізм логування часу виконання кожного виклику функції друку. Для цього використовуються часові мітки ktime_get(), які зберігаються у форматі наносекунд.
Логування реалізовано так:
```c
pr_info("Time spent: %lld ns\n", ktime_to_ns(ktime_sub(data->time_after, data->time_before)));
```

Модифікація Makefile

Для зручності налагодження було змінено файл Makefile. Було додано прапорець -g, який включає налагоджувальну інформацію в зібраному модулі. Це дозволяє легко відслідковувати місця виникнення помилок за допомогою інструментів на зразок gdb.

### Тестування

 ![](/img/1.png?raw=true)

![](/img/2.png?raw=true)

![](/img/3.png?raw=true)

![](/img/4.png?raw=true)

![](/img/5.png?raw=true)

![](/img/6.png?raw=true)

![](/img/7.png?raw=true)

