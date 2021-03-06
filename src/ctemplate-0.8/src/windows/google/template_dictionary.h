// Copyright (c) 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: Craig Silverstein
//
// Based on the 'old' TemplateDictionary by Frank Jernigan.
//
// A template dictionary maps names (as found in template files)
// to their values.  There are three types of names:
//   variables: value is a string.
//   sections: value is a list of sub-dicts to use when expanding the section;
//             the section is expanded once per sub-dict.
//   template-include: value is a list of pairs: name of the template file
//             to include, and the sub-dict to use when expanding it.
// TemplateDictionary has routines for setting these values.
//
// For (many) more details, see the doc/ directory.


#ifndef TEMPLATE_TEMPLATE_DICTIONARY_H__
#define TEMPLATE_TEMPLATE_DICTIONARY_H__

#include <stdlib.h>      // for NULL
#include <string.h>      // for the memcmp() in CompareTemplateString()
#include <stdarg.h>      // for StringAppendV()
#include <sys/types.h>   // for size_t
#include <string>
#include <vector>
#include <hash_map>
#include <google/template_modifiers.h>

// NOTE: if you are statically linking the template library into your binary
// (rather than using the template .dll), set '/D CTEMPLATE_DLL_DECL='
// as a compiler flag in your project file to turn off the dllimports.
#ifndef CTEMPLATE_DLL_DECL
# define CTEMPLATE_DLL_DECL  __declspec(dllimport)
#endif

namespace google {

class UnsafeArena;
using template_modifiers::ModifierData;

// Most methods below take a TemplateString rather than a C++ string.
// This is for efficiency: it can avoid extra string copies.
// For any argument that takes a TemplateString, you can pass in any of:
//    * A C++ string
//    * A char*
//    * TemplateString(char*, length)
// The last of these is the most efficient, though it requires more work
// on the call site (you have to create the TemplateString explicitly).
class TemplateString {
 private:
  const char*  ptr_;
  size_t       length_;

 public:
  TemplateString(const char* s) : ptr_(s ? s : ""), length_(strlen(ptr_)) {}
  TemplateString(const std::string& s) : ptr_(s.data()), length_(s.size()) {}
  TemplateString(const char* s, size_t slen) : ptr_(s), length_(slen) {}
  TemplateString(const TemplateString& s) : ptr_(s.ptr_), length_(s.length_) {}
 private:
  TemplateString();    // no empty constructor allowed
  void operator=(const TemplateString&);   // or assignment

  friend class TemplateDictionary;         // only one who can read these
};


class CTEMPLATE_DLL_DECL TemplateDictionary {
 public:
  // name is used only for debugging.
  // arena is used to store all names and values.  It can be NULL (the
  //    default), in which case we create own own arena.
  explicit TemplateDictionary(const std::string& name,
                              UnsafeArena* arena=NULL);
  ~TemplateDictionary();
  const std::string& name() const { return name_; }

  // Returns a recursive copy of this dictionary.  This dictionary
  // *must* be a "top-level" dictionary (that is, not created via
  // AddSectionDictionary() or AddIncludeDictionary()).  Caller owns
  // the resulting dict, and must delete it.  If arena is NULL, we
  // create our own.  Returns NULL if the copy fails (probably because
  // the "top-level" rule was violated).
  TemplateDictionary* MakeCopy(const std::string& name_of_copy,
                               UnsafeArena* arena=NULL);

  // --- Routines for VARIABLES
  // These are the five main routines used to set the value of a variable.
  // As always, wherever you see TemplateString, you can also pass in
  // either a char* or a C++ string, or a TemplateString(s, slen).

  void SetValue(const TemplateString variable, const TemplateString value);
  void SetIntValue(const TemplateString variable, int value);  // "%d" formatting
  void SetFormattedValue(const TemplateString variable, const char* format, ...)
#if 0
       __attribute__((__format__ (__printf__, 3, 4)))
#endif
      ;  // starts at 3 because of implicit 1st arg 'this'

  // TemplateModifier is defined in template_modifier.h, which also provides
  // some functors to use (particularly useful ones are also defined below).
  // NOTE: This method is provided for convenience, but it's better to call
  //       SetValue() and do the escaping in the template itself when possible:
  //            "...{{MYVAR:html_escape}}..."
  void SetEscapedValue(const TemplateString variable, const TemplateString value,
                       const template_modifiers::TemplateModifier& escfn);

  // NOTE: This method is provided for convenience, but it's better to call
  //       SetFormattedValue() and do the escaping in the template itself:
  //            "...{{MYVAR:html_escape}}..."
  void SetEscapedFormattedValue(const TemplateString variable,
                                const template_modifiers::TemplateModifier& escfn,
                                const char* format, ...)
#if 0
       __attribute__((__format__ (__printf__, 4, 5)))
#endif
      ;  // starts at 4 because of implicit 1st arg 'this'

  // We also let you set values in the 'global' dictionary which is
  // referenced when all other dictionaries fail.  Note this is a
  // static method: no TemplateDictionary instance needed.  Since
  // this routine is rarely used, we don't provide variants.
  static void SetGlobalValue(const TemplateString variable,
                             const TemplateString value);

  // This is used for a value that you want to be 'global', but only
  // in the scope of a given template, including all its sections and
  // all its sub-included dictionaries.  The main difference between
  // SetTemplateGlobalValue() and SetValue(), is that
  // SetTemplateGlobalValue() values persist across template-includes.
  // This is intended for session-global data; since that should be
  // fairly rare, we don't provide variants.
  void SetTemplateGlobalValue(const TemplateString variable,
                              const TemplateString value);


  // --- Routines for SECTIONS
  // We show a section once per dictionary that is added with its name.
  // Recall that lookups are hierarchical: if a section tried to look
  // up a variable in its sub-dictionary and fails, it will look next
  // in its parent dictionary (us).  So it's perfectly appropriate to
  // keep the sub-dictionary empty: that will show the section once,
  // and take all var definitions from us.  ShowSection() is a
  // convenience routine that does exactly that.

  // Creates an empty dictionary whose parent is us, and returns it.
  // As always, wherever you see TemplateString, you can also pass in
  // either a char* or a C++ string, or a TemplateString(s, slen).
  TemplateDictionary* AddSectionDictionary(const TemplateString section_name);
  void ShowSection(const TemplateString section_name);

  // A convenience method.  Often a single variable is surrounded by
  // some HTML that should not be printed if the variable has no
  // value.  The way to do this is to put that html in a section.
  // This method makes it so the section is shown exactly once, with a
  // dictionary that maps the variable to the proper value.  If the
  // value is "", on the other hand, this method does nothing, so the
  // section remains hidden.
  void SetValueAndShowSection(const TemplateString variable,
                              const TemplateString value,
                              const TemplateString section_name);

  // In this case, we hide the section if the *escaped* value of the variable
  // is the empty string.
  // NOTE: This method is provided for convenience, but it's better to call
  //       SetValueAndShowSection() and do the escaping in the template itself:
  //            "...{{MYVAR:html_escape}}..."
  void SetEscapedValueAndShowSection(const TemplateString variable,
                                     const TemplateString value,
                                     const template_modifiers::TemplateModifier& escfn,
                                     const TemplateString section_name);


  // --- Routines for TEMPLATE-INCLUDES
  // Included templates are treated like sections, but they require
  // the name of the include-file to go along with each dictionary.

  TemplateDictionary* AddIncludeDictionary(const TemplateString variable);

  // This is required for include-templates; it specifies what template
  // to include.  But feel free to call this on any dictionary, to
  // document what template-file the dictionary is intended to go with.
  void SetFilename(const TemplateString filename);

  // SetModifierData
  //   Stores data which will be accessible by modifiers when
  //   expanding this dictionary. Call with value set to NULL
  //   to clear any value previously set.
  void SetModifierData(const char* key, const void* value);

  // --- ESCAPE FUNCTORS
  // Some commonly-used escape functors.  These just point to the
  // variable of the same name in template_modifiers.h, and are kept
  // here for backwards compatibility.  For new code, and to get access
  // to escape functors not listed here, use template_modifiers.h.
  static const template_modifiers::HtmlEscape& html_escape;
  static const template_modifiers::PreEscape& pre_escape;
  static const template_modifiers::XmlEscape& xml_escape;
  static const template_modifiers::JavascriptEscape& javascript_escape;
  static const template_modifiers::UrlQueryEscape& url_query_escape;
  static const template_modifiers::JsonEscape& json_escape;


  // --- DEBUGGING TOOLS

  // Logs the contents of a dictionary and its sub-dictionaries.
  // Dump goes to stdout/stderr, while DumpToString goes to the given string.
  // 'indent' is how much to indent each line of the output.
  void Dump(int indent=0) const;
  void DumpToString(std::string* out, int indent=0) const;

  // Indicate that annotations should be inserted during template expansion.
  // template_path_start - the start of a template path.  When
  // printing the filename for template-includes, anything before and
  // including template_path_start is elided.  This can make the
  // output less dependent on filesystem location for template files.
  void SetAnnotateOutput(const char* template_path_start);


 private:
  friend class SectionTemplateNode;   // for access to GetSectionValue(), etc.
  friend class TemplateTemplateNode;  // for access to GetSectionValue(), etc.
  friend class VariableTemplateNode;  // for access to GetSectionValue(), etc.
  friend class TemplateNode;          // for access to ShouldAnnotateOutput()
  friend class Template;              // for access to ShouldAnnotateOutput()
  // For unittesting code using a TemplateDictionary.
  friend class TemplateDictionaryPeer;
  // TODO(csilvers): get these classes to use the peer class
  friend class TemplateDictionaryUnittest;
  friend class TemplateSetGlobalsUnittest;

  // The dictionary types
  inline static int TemplateStringCmp(const TemplateString& a,
                                      const TemplateString& b) {
    if (a.length_ == b.length_)
      return memcmp(a.ptr_, b.ptr_, a.length_);
    else if (a.length_ < b.length_)   // when a is shorter, 0 means <
      return memcmp(a.ptr_, b.ptr_, a.length_) <= 0 ? -1 : 1;
    else                              // when a is longer, 0 means >
      return memcmp(a.ptr_, b.ptr_, b.length_) < 0 ? -1 : 1;
  }
  struct TemplateStringEqual {
    bool operator()(const TemplateString& a, const TemplateString& b) const {
      return TemplateStringCmp(a, b) == 0;
    }
  };

  // Normally we'd put this code directly in TemplateStringHash, below, but
  // TemplateString friendship rules require the code to be at this level.
  inline static size_t HashTemplateString(const TemplateString& s) {
    // This is a terrible hash_compare-function
    unsigned long r = 0;
    for (size_t i = 0; i < s.length_; i++)
      r = 5 * r + s.ptr_[i];
    return static_cast<size_t>(r);
  }
  struct TemplateStringHash {
    size_t operator()(const TemplateString& s) const {
      return HashTemplateString(s);
    }
    // Less operator for MSVC's hash_compare containers.
    bool operator()(const TemplateString& a, const TemplateString& b) const {
      return TemplateStringCmp(a, b) < 0;
    }
    // These two public members are required by msvc.  4 and 8 are defaults.
    static const size_t bucket_size = 4;
    static const size_t min_buckets = 8;
  };
#ifdef WIN32
# define TS_HASH_MAP_(value_type) \
  stdext::hash_map<TemplateString, value_type, TemplateStringHash>
#else
# define TS_HASH_MAP_(value_type) \
  stdext::hash_map<TemplateString, value_type, TemplateStringHash, TemplateStringEqual>
#endif

  // This is a helper function to insert <key,value> into m.  Normally,
  // we'd just use m[key] == value, but hash_map rules require
  // TemplateString::operator== and default constructor to be public for
  // that to compile, and we'd rather now allow that.
  template<typename ValueType>
  static void HashInsert(TS_HASH_MAP_(ValueType)* m,
                         TemplateString key, ValueType value);

  typedef std::vector<TemplateDictionary *>  DictVector;
  typedef TS_HASH_MAP_(TemplateString) VariableDict;
  typedef TS_HASH_MAP_(DictVector*) SectionDict;
  typedef TS_HASH_MAP_(DictVector*) IncludeDict;
  // This is used only for global_dict_, which is just like a VariableDict
  typedef TS_HASH_MAP_(TemplateString) GlobalDict;
#undef TS_HASH_MAP_

  // Constructor created for all children dictionaries. This includes
  // both a pointer to the parent dictionary and also the the
  // template-global dictionary from which all children (both
  // IncludeDictionary and SectionDictionary) inherit.  Values are
  // filled into global_template_dict via SetTemplateGlobalValue.
  explicit TemplateDictionary(const std::string& name,
                              class UnsafeArena* arena,
                              TemplateDictionary* parent_dict,
                              VariableDict* template_global_dict);

  // Helps set up the static stuff
  static GlobalDict* SetupGlobalDictUnlocked();

  // This is true iff this template was created via the public constructor,
  // and not the private one used for subsidiary dicts (section/include)
  bool is_rootlevel_template() const { return template_global_dict_owner_; }

  // Utility functions for copying a string into the arena.
  TemplateString Memdup(const char* s, size_t slen);
  TemplateString Memdup(const TemplateString& s) {
    return Memdup(s.ptr_, s.length_);
  }

  // Used for recursive MakeCopy calls.
  TemplateDictionary* InternalMakeCopy(const std::string& name_of_copy,
                                       UnsafeArena* arena);

  // Used to do the formatting for the SetFormatted*() functions
  static int StringAppendV(char* space, char** out,
                           const char* format, va_list ap);

  // Was AnnotateOutput() called?  If so, what parameter was passed in?
  bool ShouldAnnotateOutput() const;
  const char* GetTemplatePathStart() const;

  // How Template::Expand() and its children access the template-dictionary.
  // TODO(csilvers): have inputs be a TemplateString rather than a string&.
  // TODO(csilvers): have GetSectionValue return a TemplateString.
  const char *GetSectionValue(const std::string& variable) const;
  bool IsHiddenSection(const std::string& name) const;
  const DictVector& GetDictionaries(const std::string& section_name) const;
  bool IsHiddenTemplate(const std::string& name) const;
  const char *GetIncludeTemplateName(const std::string& variable, int dictnum) const;
  const DictVector& GetTemplateDictionaries(const std::string& include_name) const;

  const ModifierData* modifier_data() const { return &modifier_data_; }


  // The "name" of the dictionary for debugging output (Dump, etc.)
  // The arena, also set at construction time
  std::string const name_;
  class UnsafeArena* const arena_;
  bool should_delete_arena_;   // only true if we 'new arena' in constructor

  // The three dictionaries that I own -- for vars, sections, and template-incs
  VariableDict* variable_dict_;
  SectionDict* section_dict_;
  IncludeDict* include_dict_;

  // Like variable_dict_, but persists across template-includes.
  // Unlike the global dict in that only this template and its children
  // get to see the values.  So it's halfway in between.
  VariableDict* template_global_dict_;
  bool template_global_dict_owner_;  // true iff our class new-ed template_g_d_

  // My parent dictionary, used when variable lookups at this level fail.
  // Note this is only for *variables*, not sections or templates.
  TemplateDictionary* parent_dict_;
  // The static, global dictionary, at the top of the parent-dictionary chain
  static GlobalDict* global_dict_;

  // The filename associated with this dictionary.  If set, this declares
  // what template the dictionary is supposed to be expanded with.  Required
  // for template-includes, optional (but useful) for 'normal' dicts.
  const char* filename_;

  // Data used by the modifiers and the template library during
  // Expand (ie: should we annotate the output?).
  ModifierData modifier_data_;

 private:
  // Can't invoke copy constructor or assignment operator
  TemplateDictionary(const TemplateDictionary&);
  void operator=(const TemplateDictionary&);
};

}

#endif  // TEMPLATE_TEMPLATE_DICTIONARY_H__
