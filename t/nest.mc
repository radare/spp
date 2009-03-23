Hello {{echo world {{echo :D}} ==}}-->

{{system echo $a}}
{{ifeq a {{get a}}}}
   {{set var {{get PATH}}}}
Path is {{get PATH}}
   The path is {{system echo $var}}
{{endif}}

{{set yes 1}}
{{set no 0}}

{{if yes}}
  1 INNER VISIBLE TEXT
{{if no}}
  2 BUG: INVISIBLE TEXT
   {{if no}}
{{if yes}}
TRAP
{{endif}}
   fuck
  {{endif}}
  no one loves me
{{endif}}
  3 INNER VISIBLE TEXT
{{endif}}

VISIBLE TEXT

{{if yes}}
  YES DO IT
{{endif}}

{{if no}}
  {{if yes}}
THIS IS A TRAP
  {{endif}}
  NO DONT DO IT
{{endif}}
