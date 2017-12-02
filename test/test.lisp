(set 'x 5)
(cons x '(a b c))
(set 'f (lambda (x)
          (cond
            ((= x 0) 1)
            (t (* x (f (- x 1)))))))
