// ---------------------------------------------------------------------------
//                               Работа с шифтом
// ---------------------------------------------------------------------------

Shift shift_should_be = 0;
Shift shift_current = 0;
uint32_t shift_timer = 0;
uint8_t shift_pressed_count = 0;

Key shift_get_key(Key key) {
  switch (key) {
    case KS_GRV:  return KC_GRV;
    case KS_TILD: return KC_GRV;
    case KS_1:    return KC_1;
    case KS_EXCL: return KC_1;
    case KS_2:    return KC_2;
    case KS_AT:   return KC_2;
    case KS_3:    return KC_3;
    case KS_HASH: return KC_3;
    case KS_4:    return KC_4;
    case KS_DLR:  return KC_4;
    case KS_5:    return KC_5;
    case KS_PERC: return KC_5;
    case KS_6:    return KC_6;
    case KS_CIRC: return KC_6;
    case KS_7:    return KC_7;
    case KS_AMPR: return KC_7;
    case KS_8:    return KC_8;
    case KS_ASTR: return KC_8;
    case KS_9:    return KC_9;
    case KS_LPRN: return KC_9;
    case KS_0:    return KC_0;
    case KS_RPRN: return KC_0;
    case KS_MINS: return KC_MINS;
    case KS_UNDS: return KC_MINS;
    case KS_EQL:  return KC_EQL;
    case KS_PLUS: return KC_EQL;
    case KS_Q:    return KC_Q;
    case KS_S_Q:  return KC_Q;
    case KS_W:    return KC_W;
    case KS_S_W:  return KC_W;
    case KS_E:    return KC_E;
    case KS_S_E:  return KC_E;
    case KS_R:    return KC_R;
    case KS_S_R:  return KC_R;
    case KS_T:    return KC_T;
    case KS_S_T:  return KC_T;
    case KS_Y:    return KC_Y;
    case KS_S_Y:  return KC_Y;
    case KS_U:    return KC_U;
    case KS_S_U:  return KC_U;
    case KS_I:    return KC_I;
    case KS_S_I:  return KC_I;
    case KS_O:    return KC_O;
    case KS_S_O:  return KC_O;
    case KS_P:    return KC_P;
    case KS_S_P:  return KC_P;
    case KS_LBRC: return KC_LBRC;
    case KS_LCBR: return KC_LBRC;
    case KS_RBRC: return KC_RBRC;
    case KS_RCBR: return KC_RBRC;
    case KS_A:    return KC_A;
    case KS_S_A:  return KC_A;
    case KS_S:    return KC_S;
    case KS_S_S:  return KC_S;
    case KS_D:    return KC_D;
    case KS_S_D:  return KC_D;
    case KS_F:    return KC_F;
    case KS_S_F:  return KC_F;
    case KS_G:    return KC_G;
    case KS_S_G:  return KC_G;
    case KS_H:    return KC_H;
    case KS_S_H:  return KC_H;
    case KS_J:    return KC_J;
    case KS_S_J:  return KC_J;
    case KS_K:    return KC_K;
    case KS_S_K:  return KC_K;
    case KS_L:    return KC_L;
    case KS_S_L:  return KC_L;
    case KS_SCLN: return KC_SCLN;
    case KS_COLN: return KC_SCLN;
    case KS_QUOT: return KC_QUOT;
    case KS_DQUO: return KC_QUOT;
    case KS_BSLS: return KC_BSLS;
    case KS_PIPE: return KC_BSLS;
    case KS_Z:    return KC_Z;
    case KS_S_Z:  return KC_Z;
    case KS_X:    return KC_X;
    case KS_S_X:  return KC_X;
    case KS_C:    return KC_C;
    case KS_S_C:  return KC_C;
    case KS_V:    return KC_V;
    case KS_S_V:  return KC_V;
    case KS_B:    return KC_B;
    case KS_S_B:  return KC_B;
    case KS_N:    return KC_N;
    case KS_S_N:  return KC_N;
    case KS_M:    return KC_M;
    case KS_S_M:  return KC_M;
    case KS_COMM: return KC_COMM;
    case KS_LT:   return KC_COMM;
    case KS_DOT:  return KC_DOT;
    case KS_GT:   return KC_DOT;
    case KS_SLSH: return KC_SLSH;
    case KS_QUES: return KC_SLSH;

    default: return NONE_KEY;
  }
}

Shift shift_get_shift(Key key) {
  if (KS_GRV <= key && key <= KS_QUES) {
    return (key - KS_GRV) % 2;
  } else {
    return NONE_SHIFT;
  }
}

void shift_activate(Shift shift) {
	if (shift_current != shift) {
		shift_timer = timer_read();
		if (shift) {
			register_code(KC_LSHIFT);
		} else {
			unregister_code(KC_LSHIFT);
		}
	}
	shift_current = shift;
}

void shift_activate_from_user(Shift shift) {
  shift_should_be = shift;
  shift_activate(shift);
}

Key shift_process(Key key, bool down) {
	Shift new_shift = shift_get_shift(key);
	if (down) {
		if (new_shift != NONE_SHIFT) {
			shift_activate(new_shift);
		} else {
			shift_activate(shift_should_be);
		}
	}

	if (new_shift != NONE_SHIFT) {
		if (down) {
			shift_pressed_count++;
		} else {
			shift_pressed_count--;
		}
	}

	return shift_get_key(key);
}

void shift_user_timer(void) {
	// Нужно выключать шифт после прохождения определённого времени, потому что пользователь ожидает как будто шифт на самом деле включён
	if (shift_pressed_count == 0 && shift_current != shift_should_be && timer_read() - shift_timer >= 100) {
		shift_activate(shift_should_be);
		shift_timer = timer_read();
	}
}

// ---------------------------------------------------------------------------
//                          Работа с одиночным шифтом
// ---------------------------------------------------------------------------

uint8_t shift_once_disable_stage = 2;
uint8_t shift_once_layer_off = 0;
uint8_t shift_once_layer_current = 0;
uint32_t shift_once_enabled_time = 0;
bool shift_once_can_disable = true;

bool shift_once_is_enabled(void) {
  return shift_once_disable_stage == 2;
}

void shift_once_use_to_next_key(uint8_t layer) {
  if (shift_current == 0) {
    shift_activate_from_user(true);
    layer_on(layer);
    shift_once_disable_stage = 2;
    shift_once_layer_off = layer;
    shift_once_enabled_time = timer_read();
  }
}

void shift_once_process_key(uint8_t layer, bool down) {
  if (down) {
    shift_once_use_to_next_key(layer);
    shift_once_can_disable = false;
  } else {
    shift_once_can_disable = true;
    shift_once_enabled_time = timer_read();
  }
}

void shift_once_disable(void) {
  if (shift_once_disable_stage == 2) {
    layer_off(shift_once_layer_off);
    shift_activate_from_user(false);
    shift_once_disable_stage = 0;
  }
}

void shift_once_process(Key key, keyrecord_t* record) {
  bool down = record->event.pressed;
  
  if (shift_once_disable_stage == 1) {
    shift_once_disable_stage = 0;
    shift_activate_from_user(false);
  }
  if (down && key != SFT_N_O && shift_once_disable_stage == 2) {
    shift_once_disable_stage = 1;
    layer_off(shift_once_layer_off);
  }
}

void shift_once_user_timer(void) {
  if (shift_once_can_disable && shift_once_is_enabled() && timer_read() - shift_once_enabled_time >= 1000) {
    shift_once_disable();
  }
}

// ---------------------------------------------------------------------------
//                               Работа с языком
// ---------------------------------------------------------------------------

Lang lang_should_be = 0;
Lang lang_current = 0;
uint32_t lang_timer = 0;
uint8_t lang_pressed_count = 0;

Key lang_get_key(Key key) {
  if (EN_GRV <= key && key <= EN_QUES) {
    return (key - EN_GRV) + KS_GRV;
  } else if (RU_JO <= key && key <= RU_COMM) {
    return (key - RU_JO) + KS_GRV;
  } else {
    return NONE_KEY;
  }
}

Lang lang_get_lang(Key key) {
  if (EN_GRV <= key && key <= EN_QUES) {
    return 0;
  } else if (RU_JO <= key && key <= RU_COMM) {
    return 1;
  } else {
    return NONE_LANG;
  }
}

Key lang_calc_agnostic(Key key) {
  if (lang_current == 0) {
    switch (key) {
      case AG_1:    return EN_1;
      case AG_2:    return EN_2;
      case AG_3:    return EN_3;
      case AG_4:    return EN_4;
      case AG_5:    return EN_5;
      case AG_6:    return EN_6;
      case AG_7:    return EN_7;
      case AG_8:    return EN_8;
      case AG_9:    return EN_9;
      case AG_0:    return EN_0;
      case AG_EXCL: return EN_EXCL;
      case AG_PERC: return EN_PERC;
      case AG_ASTR: return EN_ASTR;
      case AG_LPRN: return EN_LPRN;
      case AG_RPRN: return EN_RPRN;
      case AG_MINS: return EN_MINS;
      case AG_UNDS: return EN_UNDS;
      case AG_EQL:  return EN_EQL;
      case AG_PLUS: return EN_PLUS;
      case AG_SCLN: return EN_SCLN;
      case AG_COLN: return EN_COLN;
      case AG_DQUO: return EN_DQUO;
      case AG_BSLS: return EN_BSLS;
      case AG_COMM: return EN_COMM;
      case AG_DOT:  return EN_DOT;
      case AG_SLSH: return EN_SLSH;
      case AG_QUES: return EN_QUES;
      default:      return NONE_KEY;
    }
  } else {
    switch (key) {
      case AG_1:    return RU_1;
      case AG_2:    return RU_2;
      case AG_3:    return RU_3;
      case AG_4:    return RU_4;
      case AG_5:    return RU_5;
      case AG_6:    return RU_6;
      case AG_7:    return RU_7;
      case AG_8:    return RU_8;
      case AG_9:    return RU_9;
      case AG_0:    return RU_0;
      case AG_EXCL: return RU_EXCL;
      case AG_PERC: return RU_PERC;
      case AG_ASTR: return RU_ASTR;
      case AG_LPRN: return RU_LPRN;
      case AG_RPRN: return RU_RPRN;
      case AG_MINS: return RU_MINS;
      case AG_UNDS: return RU_UNDS;
      case AG_EQL:  return RU_EQL;
      case AG_PLUS: return RU_PLUS;
      case AG_SCLN: return RU_SCLN;
      case AG_COLN: return RU_COLN;
      case AG_DQUO: return RU_DQUO;
      case AG_BSLS: return RU_BSLS;
      case AG_COMM: return RU_COMM;
      case AG_DOT:  return RU_DOT;
      case AG_SLSH: return RU_SLSH;
      case AG_QUES: return RU_QUES;
      default:      return NONE_KEY;
    }
  }
}

uint8_t lang_get_shift_layer_number(void) {
  return lang_should_be * 2 + 1;
}

void lang_synchronize(void) {
  lang_timer = timer_read();
  switch (lang_current_change) {
    case LANG_CHANGE_CAPS: {
      // Костыль, потому что при нажатии Shift+Caps включается режим Caps, а не переключение языка :facepalm:
      if (shift_current == 1) {
      	unregister_code(KC_LSHIFT);
      	register_code(KC_CAPS);
      	unregister_code(KC_CAPS);
      	register_code(KC_LSHIFT);
      } else {
      	register_code(KC_CAPS);
      	unregister_code(KC_CAPS);
      }
    } break;
    case LANG_CHANGE_ALT_SHIFT: {
      register_code(KC_LALT);
      register_code(KC_LSHIFT);
      unregister_code(KC_LSHIFT);
      unregister_code(KC_LALT);

      // Костыль, потому что при зажатом шифте если хочется нажать клавишу, которая переключает язык, то шифт слетает... 
      if (shift_current == 1) {
        register_code(KC_LSHIFT);
      }
    } break;
    case LANG_CHANGE_CTRL_SHIFT: {
      register_code(KC_LCTRL);
      register_code(KC_LSHIFT);
      unregister_code(KC_LSHIFT);
      unregister_code(KC_LCTL);

      // Костыль, потому что при зажатом шифте если хочется нажать клавишу, которая переключает язык, то шифт слетает...
      if (shift_current == 1) {
        register_code(KC_LSHIFT);
      }
    } break;
    case LANG_CHANGE_WIN_SPACE: {
      register_code(KC_LGUI);
      register_code(KC_SPACE);
      unregister_code(KC_SPACE);
      unregister_code(KC_LGUI);
    } break;
  }
}

void lang_activate(Lang lang) {
	// Нужно дополнять этот код, если нужно три языка и более
	if (lang_current != lang) {
		lang_synchronize();
	}
	lang_current = lang;
}

void lang_activate_from_user(Lang lang) {
	lang_should_be = lang;
	lang_activate(lang);
}

Key lang_process(Key key, bool down) {
  Key after_agnostic = lang_calc_agnostic(key);
  if (after_agnostic != NONE_KEY) {
    key = after_agnostic;
  }

	Lang new_lang = lang_get_lang(key);
	if (down) {
		if (new_lang != NONE_LANG) {
			lang_activate(new_lang);
		} else {
			lang_activate(lang_should_be);
		}
	}

  if (new_lang != NONE_LANG) {
    if (down) {
      lang_pressed_count++;
    } else {
      lang_pressed_count--;
    }
  }

	return lang_get_key(key);
}

void lang_user_timer(void) {
	// Нужно выключать язык после прохождения определённого времени, потому что пользователь ожидает как будто шифт на самом деле включён
	if (lang_pressed_count == 0 && lang_current != lang_should_be && timer_read() - lang_timer >= 100) {
		lang_activate(lang_should_be);
	}
}

// ---------------------------------------------------------------------------
//                                Обработка клавиш
// ---------------------------------------------------------------------------

uint8_t lang_shift_current_shift_layer = 0;

void lang_shift_press_key(Key key, bool down) {
  keyrecord_t record = {
    .event = {
      .key = {
        .col = 0,
        .row = 0,
      },
      .pressed = down,
      .time = timer_read(),
    },
  };
  
  lang_shift_process_record(key, &record);
}

void lang_shift_tap_key(Key key) {
  lang_shift_press_key(key, true);
  lang_shift_press_key(key, false);
  shift_activate(shift_should_be);
  lang_activate(lang_should_be);
}

bool lang_shift_process_custom_keycodes(Key key, keyrecord_t* record) {
  bool down = record->event.pressed;

  // Обрабатываем клавиши, связанные с кастомным шифтом и кастомным переключением языка
  switch (key) {
    case SFT_N_O:
      shift_once_process_key(lang_get_shift_layer_number(), down);
      return false;
    case SFT_N:
      if (down) {
        shift_activate_from_user(true);
        lang_shift_current_shift_layer = lang_get_shift_layer_number();
        layer_on(lang_shift_current_shift_layer);
      } else {
        shift_activate_from_user(false);
        layer_off(lang_shift_current_shift_layer);
      }
      return false;
    case LA_CHNG:
      if (down) {
        if (lang_should_be == 0) {
          lang_activate_from_user(1);
          layer_on(2);  
        } else {
          lang_activate_from_user(0);
          layer_off(2);
        }
      }
      return false;
    case LA_SYNC:
      if (down) {
        lang_synchronize();
      }
      return false;
    case LA_CAPS:
      if (down) {
        lang_current_change = LANG_CHANGE_CAPS;
      }
      return false;
    case LA_ALSH:
      if (down) {
        lang_current_change = LANG_CHANGE_ALT_SHIFT;
      }
      return false;
    case LA_CTSH:
      if (down) {
        lang_current_change = LANG_CHANGE_CTRL_SHIFT;
      }
      return false;
    case LA_WISP:
      if (down) {
        lang_current_change = LANG_CHANGE_WIN_SPACE;
      }
      return false;
    case AG_3DOT:
      if (record->event.pressed) {
        lang_shift_tap_key(AG_DOT);
        lang_shift_tap_key(AG_DOT);
        lang_shift_tap_key(AG_DOT);
      }    
      return false;
      break;
    case AG_CMSP:
      if (record->event.pressed) {
        lang_shift_tap_key(AG_COMM);
        register_code(KC_SPC);
        unregister_code(KC_SPC);
      }
      return false;
      break;
    case AG_SDOT:
      if (record->event.pressed) {
        lang_shift_tap_key(AG_DOT);
        register_code(KC_SPC);
        unregister_code(KC_SPC);
        shift_once_use_to_next_key(lang_get_shift_layer_number());
      }
      return false;
      break;
  }

  return true;
}

bool lang_shift_process_english_modifiers(Key key, keyrecord_t* record) {
  static Lang lang_stack[3] = {};
  static uint8_t modifiers_count = 0;
  #define PROCESS(NAME, REGISTER, UNREGISTER) \
    case NAME: { \
      if (record->event.pressed) { \
        lang_stack[modifiers_count] = lang_should_be; \
        modifiers_count += 1; \
        if (lang_should_be == 1) { \
          layer_off(2); \
        } \
        lang_activate_from_user(0); \
        REGISTER; \
      } else { \
        UNREGISTER; \
        modifiers_count -= 1; \
        lang_activate_from_user(lang_stack[modifiers_count]); \
        if (lang_should_be == 1) { \
          layer_on(2); \
        } \
      } \
      return false; \
    } break;

  #define Rg(x) register_code(KC_L ## x)
  #define Un(x) unregister_code(KC_L ## x)

  switch (key) {
    PROCESS(CTRL_0, Rg(CTRL), Un(CTRL));
    PROCESS(ALT_0,  Rg(ALT),  Un(ALT));
    PROCESS(WIN_0,  Rg(GUI),  Un(GUI));
    PROCESS(CTAL_0, { Rg(CTRL);  Rg(ALT);   }, { Un(ALT);   Un(CTRL);  })
    PROCESS(SHAL_0, { Rg(SHIFT); Rg(ALT);   }, { Un(ALT);   Un(SHIFT); })
    PROCESS(CTSH_0, { Rg(CTRL);  Rg(SHIFT); }, { Un(SHIFT); Un(CTRL);  })
    PROCESS(MCAS_0, { Rg(CTRL);  Rg(ALT); Rg(SHIFT); }, { Un(SHIFT); Un(ALT); Un(CTRL); })
  }

  return true;
}

bool lang_shift_process_record(Key key, keyrecord_t* record) {
  // Обрабатываем Once Shift
  shift_once_process(key, record);

  bool down = record->event.pressed;

  // Разбираемся, имеет ли эта клавиша какой-то язык, заданный в ней
  Key key1 = lang_process(key, down);
  Key key_to_shift = key;
  if (key1 != NONE_KEY) {
    key_to_shift = key1;
  }

  // Разбираемся, имеет ли эта клавиша шифт, засунутый в неё
  // Это нужно отдельно от обработки языка, чтобы шифт мог выключаться для обычных клавиш
  Key key2 = shift_process(key_to_shift, down);
  if (key2 != NONE_KEY) {
    if (down) {
      register_code(key2);
    } else {
      unregister_code(key2);
    }
    return false;
  }

  if (!lang_shift_process_custom_keycodes(key, record)) {
    return false;
  }

  if (!lang_shift_process_english_modifiers(key, record)) {
    return false;
  }

  return true;
}

void lang_shift_user_timer(void) {
	shift_user_timer();
	shift_once_user_timer();
	lang_user_timer();
}
