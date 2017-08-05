# xv6

This is an implementation of MLFQ scheduler, virtual memory layout rearrangement and kernel threads of xv6 operation system

The VM layout now looks like

+-------------------------------+  <--- USERTOP
|     Stack of the Process      |  1 PAGE
+-------------------------------+  
|      Guard Page (FREE)        |  1 PAGE
+-------------------------------+
|      Stack of Thread 1        |  1 PAGE
+-------------------------------+
|      Guard Page (FREE)        |  1 PAGE
+-------------------------------+
|                               |
:                               :
:              ^                :
|              |                |
+-------------------------------+  <--- proc->sz
|            Heap               | 
+-------------------------------+
|         Code + Data           |  1 PAGE
+-------------------------------+
|      Guard Page (FREE)        |  2 PAGES
+-------------------------------+
