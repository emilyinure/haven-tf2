#pragma once

// forward decl.
class con_command_base;
typedef int cvar_dll_identifier_t;
class i_convar;
class c_command;

typedef void (*fn_change_callback_t)(i_convar* var, const char* old_value, float f_old_value);
typedef void (*fn_command_callback_void_t)(void);
typedef void (*fn_command_callback_t)(const c_command& command);
typedef int (*fn_command_completion_callback)(const char* partial, char commands[64][64]);

class i_con_command_base_accessor {
public:
  virtual bool register_con_command_base(con_command_base* var) = 0;
};

class i_convar {
public:
  virtual void        set_value(const char* p_value)       = 0;
  virtual void        set_value(float value)               = 0;
  virtual void        set_value(int value)                 = 0;
  virtual void        set_value(color value)               = 0;
  virtual const char* get_name(void) const                 = 0;
  virtual const char* get_base_name(void) const            = 0;
  virtual bool        is_flag_set(int n_flag) const        = 0;
  virtual int         get_split_screen_player_slot() const = 0;
};

class con_command_base {
public:
  virtual ~con_command_base(void);
  virtual bool                  is_command(void) const;
  virtual bool                  is_flag_set(int flag) const;
  virtual void                  add_flags(int flags);
  virtual void                  remove_flags(int flags);
  virtual int                   get_flags() const;
  virtual const char*           get_name(void) const;
  virtual const char*           get_help_text(void) const;
  virtual bool                  is_registered(void) const;
  virtual cvar_dll_identifier_t get_dll_identifier() const;
  virtual void create(const char* name, const char* help_string = 0, int flags = 0);
  virtual void init();

  con_command_base* m_next;
  bool              m_registered;
  const char*       m_name;
  const char*       m_help_string;
  int               m_flags;
};

class c_command {
public:
  c_command();
  c_command(int n_arg_c, const char** pp_arg_v);

private:
  enum {
    command_max_argc   = 64,
    command_max_length = 512,
  };

  int         m_argc;
  int         m_argv0_size;
  char        m_arg_s_buffer[command_max_length];
  char        m_argv_buffer[command_max_length];
  const char* m_argv[command_max_argc];
};

class i_command_callback {
public:
  virtual void command_callback(const c_command& command) = 0;
};

class i_command_completion_callback {
public:
  virtual int command_completion_callback(const char*                 partial,
                                          c_utl_vector<c_utl_string>& commands) = 0;
};

class con_command : public con_command_base {
public:
  virtual ~con_command(void);
  virtual bool is_command(void) const;
  virtual int  auto_complete_suggest(const char* partial, c_utl_vector<c_utl_string>& commands);
  virtual bool can_auto_complete(void);
  virtual void dispatch(const c_command& command);

  union {
    fn_command_callback_void_t m_fn_command_callback_v1;
    fn_command_callback_t      m_fn_command_callback;
    i_command_callback*        m_p_command_callback;
  };
  union {
    fn_command_completion_callback m_fn_completion_callback;
    i_command_completion_callback* m_p_command_completion_callback;
  };

  bool m_has_completion_callback;
  bool m_using_new_command_callback;
  bool m_using_command_callback_interface;
};

class convar : public con_command_base, public i_convar {
public:
  typedef con_command_base base_class;
  virtual ~convar(void);
  virtual bool        is_flag_set(int flag) const;
  virtual const char* get_help_text(void) const;
  virtual bool        is_registered(void) const;
  virtual const char* get_name(void) const;
  virtual const char* get_base_name(void) const;
  virtual int         get_split_screen_player_slot() const;
  virtual void        add_flags(int flags);
  virtual int         get_flags() const;
  virtual bool        is_command(void) const;
  virtual void        set_value(const char* value);
  virtual void        set_value(float value);
  virtual void        set_value(int value);
  virtual void        set_value(color value);

  struct cv_value_t {
    char* m_string_value;
    int   m_string_length;
    float m_float_value;
    int   m_int_value;
  };

  virtual void internal_set_value(const char* value);
  virtual void internal_set_float_value(float f_new_value);
  virtual void internal_set_int_value(int n_value);
  virtual void internal_set_color_value(color value);
  virtual bool clamp_value(float& value);
  virtual void change_string_value(const char* temp_val, float fl_old_value);
  virtual void create(const char* name, const char* default_value, int flags = 0,
                      const char* help_string = 0, bool min = false, float f_min = 0.0,
                      bool b_max = false, float f_max = false,
                      fn_change_callback_t callback = 0);
  virtual void init();

  convar*                            m_parent;
  const char*                        m_default_value;
  cv_value_t                         m_value;
  bool                               m_has_min;
  float                              m_min_val;
  bool                               m_has_max;
  float                              m_max_val;
  c_utl_vector<fn_change_callback_t> m_change_callbacks;
};
