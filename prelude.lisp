; Lisp Prelude
; Core macros and standard library implemented in Lisp
; on top of the minimal C primitives:
;   quote atom eq car cdr cons cond set defmacro env lambda
;   + - * / % = > >= < <=
;   print strcat strlen
;   mkvec vref vset vlen hash

;; ---- Variadic bootstrap ----

; list must be defined first (macros use it in their expansions)
(set 'list (lambda args args))

;; ---- Core macros ----

; define: bind a name without quoting
(defmacro define (name value)
  (list 'set (list 'quote name) value))

; defun: define a named function (supports multiple body expressions)
; single body:  (defun name (params) body)
; multi body:   (defun name (params) body1 body2 ...)
(defmacro defun args
  (cond ((atom (cdr (cdr args)))
         (cond ((atom (cdr args)) '())
               (t '())))
        ((atom (cdr (cdr (cdr args))))
         (list 'define (car args)
               (list 'lambda (car (cdr args)) (car (cdr (cdr args))))))
        (t (list 'define (car args)
                 (list 'lambda (car (cdr args))
                       (cons 'progn (cdr (cdr args))))))))

;; ---- Let ----

; Helpers to extract names and values from binding lists
(defun map-car (lst)
  (cond ((atom lst) '())
        (t (cons (car (car lst)) (map-car (cdr lst))))))

(defun map-cadr (lst)
  (cond ((atom lst) '())
        (t (cons (car (cdr (car lst))) (map-cadr (cdr lst))))))

; let: local bindings via lambda application
; (let ((x 1) (y 2)) body) => ((lambda (x y) body) 1 2)
(defmacro let (bindings body)
  (cons (list 'lambda (map-car bindings) body)
        (map-cadr bindings)))

;; ---- Boolean operators ----

(defun not (x) (cond (x '()) (t t)))

(defmacro and (a b)
  (list 'cond (list a b) (list t '())))

(defmacro or (a b)
  (list 'cond (list a a) (list t b)))

(defmacro when (pred body)
  (list 'cond (list pred body)))

(defmacro unless (pred body)
  (list 'cond (list pred '()) (list t body)))

;; ---- Predicates ----

(defun null? (x) (eq x '()))

;; ---- Sequencing ----

(defmacro progn body
  (cond ((null? (cdr body)) (car body))
        (t (list 'cond
                 (list (list 'list (car body))
                       (cons 'progn (cdr body)))))))

;; ---- List accessors ----

(defun cadr (x) (car (cdr x)))
(defun caddr (x) (car (cdr (cdr x))))

;; ---- Higher-order functions ----

(defun map (f lst)
  (cond ((atom lst) '())
        (t (cons (f (car lst)) (map f (cdr lst))))))

(defun filter (f lst)
  (cond ((atom lst) '())
        ((f (car lst)) (cons (car lst) (filter f (cdr lst))))
        (t (filter f (cdr lst)))))

(defun reduce (f acc lst)
  (cond ((atom lst) acc)
        (t (reduce f (f acc (car lst)) (cdr lst)))))

;; ---- List utilities ----

(defun append (a b)
  (cond ((atom a) b)
        (t (cons (car a) (append (cdr a) b)))))

(defun length (lst)
  (cond ((atom lst) 0)
        (t (+ 1 (length (cdr lst))))))

(defun reverse (lst)
  (reduce (lambda (acc x) (cons x acc)) '() lst))

(defun nth (lst n)
  (cond ((= n 0) (car lst))
        (t (nth (cdr lst) (- n 1)))))

;; ---- Hash Map ----
;; Implemented as a vector of association lists (separate chaining)

(defun make-hashmap () (mkvec 16))

(defun hm-bucket (m key)
  (% (hash key) (vlen m)))

; alist helpers
(defun alist-get (key lst)
  (cond ((atom lst) '())
        ((eq (car (car lst)) key) (cadr (car lst)))
        (t (alist-get key (cdr lst)))))

(defun alist-set (key val lst)
  (cond ((atom lst) (list (list key val)))
        ((eq (car (car lst)) key)
         (cons (list key val) (cdr lst)))
        (t (cons (car lst) (alist-set key val (cdr lst))))))

(defun alist-del (key lst)
  (cond ((atom lst) '())
        ((eq (car (car lst)) key) (cdr lst))
        (t (cons (car lst) (alist-del key (cdr lst))))))

; public API
(defun hashmap-get (m key)
  (alist-get key (vref m (hm-bucket m key))))

(defun hashmap-set (m key val)
  (let ((idx (hm-bucket m key)))
    (vset m idx (alist-set key val (vref m idx)))))

(defun hashmap-del (m key)
  (let ((idx (hm-bucket m key)))
    (vset m idx (alist-del key (vref m idx)))))
