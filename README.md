# Кастомные русская и английская раскладки

Данный модуль позволяет вам:
* Задавать кастомную английскую и русскую раскладку прямо в прошивке клавиатуры.
	* То есть вы сможете прямо внутри клавиатуры запрограммировать раскладку Диктор и Дворак для программистов.
* Быть полностью независимым от софта. То есть данная раскладка будет работать на любой системе, на абсолютно стандартных раскладках QWERTY и ЙЦУКЕН, и вам не нужно создавать и устанавливать свою раскладку в MKLC, например.
	* Утверждение верно только для Windows, GNU/Linux, Android; на продукции от Apple я не тестировал, говорят там вообще по умолчанию машинописная раскладка для русского языка.
* Нажимать символы с шифтом, когда включён основной слой (можно симулировать раскладку Русская Машинопись).
* Нажимать символы без шифта, когда нажат шифт (можно симулировать ту часть Русской Машинописи, когда при зажатом шифте можно набирать цифры).
* Нажимать английские символы при включённой русской раскладке и наоборот. Это позволит вам создать раскладко-независимый способ набирать символы, и больше вам не придётся переключать язык чтобы набрать квадратные скобки,  `` ` `` из русского или `№` из английского.
* При нажатии модификаторов Ctrl, Alt, Win etc. раскладка клавиатуры будет считаться английской, и у вас не будет проблемы в том что на русском языке хоткеи расположены в одном месте, а на английском языке по-другому.
* Использовать особый шифт, который применяется только к следующей клавише, и использовать его в составе собственных макросов, например можно создать макрос, который набирает вопросительный знак, затем пробел, и затем автоматически применяет шифт к следующей нажатой клавише.

Обратите внимание, что данный модуль работает через расширение пользовательских клавиш, то есть многие фичи QMK что работают с `KC_A` не будут работать с `EN_A`, например [Combos](https://docs.qmk.fm/#/feature_combo).

# Как использовать?

## Задать характеристики

Нужно задать способ переключения языка по умолчанию в переменной препроцессора `LANG_CHANGE_DEFAULT` в файле `config.h`, используя один из вариантов:

```
LANG_CHANGE_CAPS
LANG_CHANGE_ALT_SHIFT
LANG_CHANGE_CTRL_SHIFT
LANG_CHANGE_WIN_SPACE
```

То есть в файле `config.h` должна быть строка следующего вида:
```
#define LANG_CHANGE_DEFAULT LANG_CHANGE_ALT_SHIFT
```

Так же этими вариантами можно задавать способ переключения языка во время работы раскладки, модифицируя переменную `lang_current_change`, либо прямо на раскладке используя кейкоды:

```c
LA_CAPS, /* Задаёт переключение языка на Caps. */
LA_ALSH, /* Задаёт переключение языка на Alt + Shift. */
LA_CTSH, /* Задаёт переключение языка на Ctrl + Shift. */
LA_WISP, /* Задаёт переключение языка на Win + Space. */
```

## Задание SAFE_RANGE

Для пользовательских кейкодов существует такое понятие как `SAFE_RANGE`, обычно он используется для того чтобы после него помещать свои кейкоды, которые делают особые действия:

```c
enum custom_keycodes {
  MY_KEY1 = SAFE_RANGE,
  MY_KEY2,
  // ...
}
```

Таким образом гарантируется что пользовательские кейкоды не пересекутся с системными кейкодами. Обычно для этого используется `SAFE_RANGE`.

Но иногда случается так, что клавиатура добавляет своих кейкодов, и создаёт новую переменную SAFE_RANGE, которую называет по-другому, нужно за этим следить. Посмотрите, нету ли у вас такой особой переменной. Например, в Moonlander эта переменная называется `ML_SAFE_RANGE`.

После того как вы найдёте имя этой переменной, в `keymap.c` прямо перед включением `lang_shift` нужно написать:

```c
#define CUSTOM_SAFE_RANGE <ваша переменная>
```

Пример для Moonlander:
```c
#define CUSTOM_SAFE_RANGE ML_SAFE_RANGE
```

Затем при использовании своих кейкодов необходимо указывать именно `CUSTOM_SAFE_RANGE`, потому что данный модуль использует необходимое количество кейкодов из пользовательских кейкодов и переопределяет эту переменную. Пример:

```c
enum custom_keycodes {
  KEYCODES_START = CUSTOM_SAFE_RANGE,

  // English specific keys
  EN_LTEQ, // <=
  // ...
};
```

## Подключить код

В своём файле `keymap.c` в самом верху подключаем файл `lang_shift/include.h`:
```c
#include "lang_shift/include.h"
```

## Вызов модуля для каждой клавиши

В своей функции `process_record_user` включаете добавляете следующую обработку:

```c
bool process_record_user(uint16_t key, keyrecord_t *record) {
  if (!lang_shift_process_record(keycode, record))
    return false;

  ...
}
```

Затем нужно определить функцию `matrix_scan_user`, если она у вас ещё не определена, и вызывать функцию `user_timer`, а в этой функции вызывать `lang_shift_user_timer();`:
```c
void user_timer(void) {
  lang_shift_user_timer();
}

void matrix_scan_user(void) {
  user_timer();
}
```

**Объяснение:** функция `matrix_scan_user` вызывается примерно каждые 2 миллисекунды, она сканирует матрицу. Значит её вполне можно использовать для отслеживания собственных таймеров. Поэтому мы вызываем из неё функцию `user_timer`, которая лучше говорит о наших намерениях, чем `matrix_scan_user`. А уже в функции `user_timer` мы вызываем обработку случая автоматического отжатия шифта или обратного переключения языка

## Настройка раскладки

Пишите свои раскладки следующим образом: 
* Первая раскладка без зажатого шифта находится в слое 0
* Первая раскладка при зажатом шифте находится в слое 1
* Аналогично вторая без шифта в 2 слое
* Аналогично вторая с шифтом в 3 слое

Причём в шифтовом слое для всех буквенных символов надо прописывать что будет нажат шифт. 

А для шифта и переключения языка используете кейкоды `SFT_N`, `LA_CHNG`, etc.

Если вы будете использовать обычный шифт вместо `SFT_N` или `SFT_N_O`, то у вас в шифтомом слое не будут работать цифры, маленькие буквы и некоторые препинаки. Данные шифты отслеживают когда были нажаты и специально для кейкода цифр, например, отжимают шифт на время их нажатия. Поэтому обычный кейкод `KC_LSFT` использовать нельзя.

Для английского и русского языка используйте кейкоды с префиксами `EN_*`, `RU_*`. Для препинаков, которые не зависят от языка, используйте Agnostic кейкоды `AG_*`.

Для модификаторов используйте клавиши `CTRL_0`, `ALT_0`, etc. Они включают 0-й слой с английским языком, чтобы все хоткеи нажимались исключительно на английской раскладке.

Все кейкоды смотрите в файле `include.h`, у некоторых кейкодов там подробное описание.

Пример очень маленькой раскладки для клавиатуры с 4 кнопками

```c
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    //---------------------------------------------------------------------------
  [0] = LAYOUT(EN_AMPR, EN_W,    SFT_N,   LA_CHNG), // & w <Шифт> <Переключение языка>
  [1] = LAYOUT(EN_7,    EN_S_W,  _______, RU_NUME), // 7 W <Шифт> №
  [2] = LAYOUT(RU_7 ,   RU_JU,   SFT_N,   LA_CHNG), // 7 ю <Шифт> <Переключение языка>
  [3] = LAYOUT(RU_QUES, RU_S_JU, _______, EN_GRV ), // ? Ю <Шифт> `
};
```

Обратите внимание, что в шифтовом слое обязательно нужно прописывать что под `ю` находится `Ю` с зажатым шифтом.

# Рассинхрон

Это работает на основе того что клавиатура внутри хранит какой сейчас язык на компьютере. Затем при использовании кейкода `LA_CHNG` одновременно отправляется запрос на переключение языка в компьютер и язык переключается в клавиатуре. Но часто бывает так что в компьютере язык поменялся не из-за клавиатуры. Поэтому приходится вручную говорить клавиатуре об этом. Для этого есть кейкод `SYNC_LA`.

# Once Shift

Здесь так же представлены клавиши `SFT_N_O`, `SFT_1_O`, `SFT_3_O` (SHiFt Once), которые работают следующим образом:
* При зажатии применяют шифт только к одной следующей клавише, то есть он автоматически выключается после срабатывания на одной клавише.
* При одиночном нажатии тоже срабатывает для следующей клавиши, как One Shot Shift, но без таймаута.

Такая клавиша может пригодиться если вы печатаете с большой скоростью и вам сложно выдерживать время удержания шифта, и у вас получается что-то вроде "ПРивет" или "привет" (шифт отпустился раньше начала следующего слова).

Так же существует клавиша `AG_SDOT`, которая отправляет: точку, пробел и применяет шифт к следующей клавише. Идеальна для использования при наборе текста.

# Плюсы и минусы по сравнению с заданием раскладки в ОС

**Плюсы:**
* Будет без всякой настройки компьютера работать, если уже установлены стандартные русский и английский языки. Вставил, нажал кнопку выбора метода переключения языка и всё.
* Будет работать без прав администратора.
* Будет работать на телефоне (на телефоне обычно очень сложно настраивать раскладку подключаемой физической клавиатуры, а на apple телефонах так вообще вроде невозможно).

**Минусы:**
* Невозможно посылать кейкоды, не существующие ни в русской, ни в английской раскладке, например `« » —`.
* Необходимость компилировать свою раскладку из кода, и вообще притрагиваться к коду. Некоторые люди предпочитают работать исключительно в графических конфигураторах.
* Ввод английских символов при включённой русской раскладке может тормозить из-за тормознутого метода переключения раскладки. Из-за этого же иногда некоторые клавиши могут прожиматься не на том языке.

# Использование в коде

Всё что вы можете использовать в коде можно посмотреть в файле `include.h`.

## Обычная клавиша на шифтовом слое

Если вы хотите использовать кастомный кейкод на шифтовом слое, но чтобы он был без зажатого шифта, то нужно в коде этого кейкода перед его активацией написать: `shift_activate(0);`. Данный код вырубает шифт, но таким образом что шифт потом сам включится через 200мс, или если он понадобится другой клавише.

Пример использования:
```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
	  case CT_Y:
	    if (record->event.pressed) {
	      shift_activate(0);
	      register_code(KC_LCTRL);
	      register_code(KC_Y);
	      unregister_code(KC_Y);
	      unregister_code(KC_LCTRL);
	    }
	    return false;
	    break;
	}
	return true;
}
```

## Англоязычный хоткей

Если вы хотите чтобы перед нажатием вашего хоткея был включён английский язык, то можно перед его нажатием вызывать функцию `lang_activate(0);`. Это будет работать аналогично верхнему пункту. 

Пример использования:
```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
	  case CT_D:
	    if (record->event.pressed) {
	      lang_activate(0);
	      register_code(KC_LCTRL);
	      register_code(KC_D);
	      unregister_code(KC_D);
	      unregister_code(KC_LCTRL);
	    }
	    return false;
	    break;
	}
	return true;
}
```

## Набрать символ на нужном языке

Если вы хотите набрать символ из определённого языка, при этом не зная какой включен сейчас, можно использовать функцию `lang_shift_tap_key`. Аналогично можно использовать функцию `lang_shift_press_key` если вам нужно этот символ зажать или отжать.

Пример использования:
```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
	  case EN_LTEQ:
	    if (record->event.pressed) {
	      lang_shift_tap_key(EN_LT);
	      lang_shift_tap_key(AG_EQL);
	    }
	    return false;
	    break;
	}
	return true;
}
```

## Применить шифт к следующей клавише

Например, вы хотите создать клавишу `AG_QSSP`, которая нажимает: <вопросительный знак> + <пробел> + <применить шифт к следующей клавише>, то вы можете использовать функцию `shift_once_use_to_next_key`, где в параметрах нужно указать на какой слой нужно переключить следующую клавишу. Если это русский язык, то нужно указать 3; если английский - 0; если вы хотите чтобы оно автоматически подстраивалось под ваш язык, то можно вызвать её с другой функцией: `shift_once_use_to_next_key(lang_get_shift_layer_number())`. В итоге код получится такой:

```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
	  case AG_QSSP:
	    if (record->event.pressed) {
	      lang_shift_tap_key(AG_QUES);
	      register_code(KC_SPC);
	      unregister_code(KC_SPC);
	      shift_once_use_to_next_key(lang_get_shift_layer_number());
	    }
	    return false;
	    break;
	}
	return true;
}
```

# Changelog

**25.01.2021:**
* Теперь клавиши `CTRL_0` итд только включают 0-й слой при их зажатии. Клавиши же `CTRL_EN` итд не только включают 0-й слой, но и переключают язык на английский.

**23.01.2021:**
* Пофикшен баг, при котором, если зажать `&`, который находится в шифтовом слое, то после отжатия шифта продолжает набираться `7`: `<Shift down><& down>&&&&&&&&&&&&<Shift up>77777777777<& up>`, теперь набирается нормально: `<Shift down><& down>&&&&&&&&&&&&<Shift up>&&&&&&&&&&&<& up>`.
