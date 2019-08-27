;;; arbel.el --- ARBEL Emacs Lisp Mode               -*- lexical-binding: t; -*-

;; Copyright (C) 2019  

;; Author:  Zach Flynn <zlflynn@gmail.com>
;; Keywords: languages

(regexp-quote "\$[^\s]*")


(defvar arbel-mode-syntax-table nil "Syntax table for `arbel-mode'.")

(setq arbel-mode-syntax-table
      (let ((synTable (make-syntax-table)))
        ;; python style comment: “# …”
        (modify-syntax-entry ?# "<" synTable)
        (modify-syntax-entry ?\n ">" synTable)
        synTable))

(setq arbel-font-lock-keywords
      (let* (
	     (x-functions '("set"
			    "get"
			    "add"
			    "multiply"
			    "subtract"
			    "while"
			    ))
	     (x-functions-regexp (regexp-opt x-functions 'words)))
	`(
	  (,x-functions-regexp . font-lock-builtin-face)
	  ("\\(\$[^\s]*\\)\s" . (1 font-lock-function-name-face))
	  )))

(define-derived-mode arbel-mode lisp-mode "arbel mode"
  "Major mode for editing code in the ARBEL language"
  (setq font-lock-defaults '((arbel-font-lock-keywords))))
