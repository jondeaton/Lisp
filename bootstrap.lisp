;;; Lisp Bootstrapper

;;; null - tests whether its argument is the empty list
(defun null (x)
  (eq x '()))

;;; and - returns t if both its arguments do and () otherwise
(defun and (x y)
  (cond (x (cond (y 't) ('t '())))
        ('t '())))

;;; not - returns t if its argument returns () and () if
;;; its argument returns t
(defun not (x)
  (cond (x '())
        ('t 't)))

;;; append - takes two lists and returns their concatenation
(defun append (x y)
  (cond ((null x) y)
        ('t (cons car x) (append (cdr x) y))))

;;; pair - takes two lists of the same length and returns a list of
;;; two-element lists containing successive pairs of an element from each.
(defun pair (x y)
  (cond ((and (null x) (null y)) '())
        ((and (not (atom x)) (not (atom y)))
         (cons (list (cat x) (car y))
               (pair (cdr x) (cdr y))))))

;;; assoc - Takes an atom x and a list y of the form created by pair
;;; and returns the second element of the first list in y whose
;;; element is x.
(defun assoc (x y)
  (cond ((eq (caar y) x) (cadar y))
        ('t (assoc x (cdr y)))))

;;; Evaluate cons
(defun evcon (c a)
  (cond ((eval (caar c) a)
         (eval (cadar c) a))
        ('t (evcon (cdr c) a))))

;;; Evaluate list
(defun evlis (m a)
  (cond ((null m) '())
        ('t (cons (eval (car m) a)
                  (evlis (cdr m) a)))))

;;; Metacircular evaluator
(defun eval (e a)
  (cond
    ((atom e) (assoc. e a))
    ((atom (car e)))
      (cond
        ((eq (car e) 'quote)  (cadr e))
        ((eq (car e) 'atom)   (atom   (eval (cadr e) a)))
        ((eq (car e) 'eq)     (eq     (eval (cadr e) a)
                                      (eval (caddr e) a)))
        ((eq (car e) 'car)     (car   (eval (cadr e) a)))
        ((eq (car e) 'cdr)     (cdr   (eval (cadr e))))
        ((eq (car e) 'cons)    (cons  (eval (cadr e) a)
                                      (eval (caddr e) a)))
        ((eq (car e) 'cond)    (evcon (cdr e) a))
        ('t (eval (cons (assoc (car e) a)
                        (cdr e))
                  a))))
  ((eq (caar e) 'defun)
   (eval (label (cadar e)
                (lambda (caddar e) (cadddar e))) a)) 
  ((eq (caar e) 'label)
   (eval (cons (caddar e) (cdr e))
         (cons (list (cadar e) (car e)) a)))
  ((eq (caar e) 'lambda)
   (eval (caddar e)
         (append (pair (cadar e) (evlis (cdr e) a))
                 a))))

