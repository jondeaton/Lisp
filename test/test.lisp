(set 'repeat (lambda (item n) 
               (cond 
                 ((= n 1) item) 
                 (t (cons (car item) (repeat item (- n 1)))))))

(set 'factorial (lambda (x)  
                  (cond 
                    ((= x 0) 1) 
                    ((= 1 1) (* x (factorial (- x 1)))))))


(set 'ith (lambda (x i) 
            (cond 
              ((= i 0) (car x)) 
              ((= 1 1) (ith (cdr x) (- i 1))))))

(set 'x (factorial 5))
(set 'y (factorial 6))
(/ y x)

(set 'Y (lambda (H) 
          ((lambda (x) (H (x x))) 
           (lambda (x) (H (x x))))))

(set 'F (lambda (G) 
          (lambda (x)  
            (cond 
              ((= x 0) 1) 
              ((= 1 1) (* x (G (- x 1))))))))

