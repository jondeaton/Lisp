
(set 'Y
     (lambda (r)
       ((lambda (f) (f f))
        (lambda (f) (r (lambda (x) ((f f) x)))))))

(set 'F
     (lambda (g)
       (lambda (n)
         (cond
           ((= n 0) 1)
           (t       (* n (g (- n 1))))))))


((Y F) 5)
