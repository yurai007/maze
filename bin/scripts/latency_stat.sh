cat log.txt | grepc "tick" | sed 1d | awk -F "::" '{ x = $2; y = sub("\].*$", "", x); print x;}' | awk -f ../../latency_helper.awk > stats_512.txt
