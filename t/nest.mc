Hello {{echo world {{echo :D}} ==}}-->

{{set a secret message}}
{{system echo $a}}
{{ifeq a {{get a}}}} /* always true */
   {{set var {{get PATH}}}}
   Path is {{get var}}
   Path is {{get PATH}}
   Path is {{system echo $var}}
   {{echo Path is {{get PATH}}}}
{{endif}}

{{set yes 1}}
{{set no 0}}

{{if yes}}
  1 INNER VISIBLE TEXT
  {{if no}}
    2 BUG: INVISIBLE TEXT
    {{if no}}
      fuck
      {{if yes}}
        TRAP
      {{endif}}
      fuck
    {{endif}}
  no one loves me
  {{endif}}
  {{if yes}}
  HELLO WORLD
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
