Hello {{echo world {{echo :D}} ==}}-->

{{system echo $a}}
{{ifeq a {{get a}}}}
   {{set var {{get PATH}}}}
   The path is {{get var}}
{{endif}}

{{if 0}}
BUG: INVISIBLE TEXT
{{if 1}}
BUG: INVISIBLE TEXT
{{endif}}
BUG: INVISIBLE TEXT
{{endif}}
VISIBLE TEXT
