; Lisp Prelude
; Core macros and standard library implemented in Lisp
; on top of the minimal C primitives:
;   quote atom eq car cdr cons cond set defmacro env lambda
;   + - * / % = > >= < <=

;; ---- Variadic bootstrap ----

; list must be defined first (macros use it in their expansions)
(set 'list (lambda args args))

;; ---- Core macros ----

; define: bind a name without quoting
(defmacro define (name value)
  (list 'set (list 'quote name) value))

; defun: define a named function
(defmacro defun (name params body)
  (list 'define name (list 'lambda params body)))

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
