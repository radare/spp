{{ set arch x86-32 }}
   ---

hello {{echo world}}
path={{echo $PATH}}
arch = {{ get arch }}

{{ ifeq arch x86-32 }}
FOO IS ENABLED
{{ endif }}

   ---
