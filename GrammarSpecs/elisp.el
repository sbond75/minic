;; Utilities: ;;

;; Copy lines matching a regex to the clipboard:

;; https://stackoverflow.com/questions/2289883/emacs-copy-matching-lines
(defun my-copy-lines-matching-re (re)
  "find all lines matching the regexp RE in the current buffer
putting the matching lines in a buffer named *matching*"
  (interactive "sRegexp to match: ")
  (let ((result-buffer (get-buffer-create "*matching*")))
    (with-current-buffer result-buffer 
      (erase-buffer))
    (save-match-data 
      (save-excursion
        (goto-char (point-min))
        (while (re-search-forward re nil t)
          (princ (buffer-substring-no-properties (line-beginning-position) 
                                                 (line-beginning-position 2))
                 result-buffer))))
    (pop-to-buffer result-buffer)))

;; My modification to the above, so that it grabs the text from the start of the line to the end of the matched regexp. By the way, C-M-% to do regex replace ( https://www.gnu.org/software/emacs/manual/html_node/emacs/Query-Replace.html , https://www.gnu.org/software/emacs/manual/html_node/emacs/Regexp-Replace.html , https://www.gnu.org/software/emacs/manual/html_node/elisp/Regexp-Functions.html ). You can even do \( and \) to mark regions that can be replaced with custom text, using \1, \2, and so on to refer to each ith region! (example regex: `<\([a-z]+\)> -> → \1 :` or `\([a-z]+\) : → mpc_parser_t\* \1` or `\([a-z_]+\) : → mpc_parser_t* \1 = mpc_new("\1");` (note: you must go to the top of the file or where you want to start running from onwards! Then do y or space to accept that one, or "!" to replace ALL in this mode.) ) :
(defun my-copy-matches-of-re (re)
  "find all matches of the regexp RE in the current buffer
putting the matches in a buffer named *matching*"
  (interactive "sRegexp to match: ")
  (let ((result-buffer (get-buffer-create "*matching*")))
    (with-current-buffer result-buffer 
      (erase-buffer))
    (save-match-data 
      (save-excursion
        (goto-char (point-min))
        (while (re-search-forward re nil t)
          (princ (concat (buffer-substring-no-properties (line-beginning-position) 
							 (point)) "\n")
                 result-buffer))))
    (pop-to-buffer result-buffer)))

;; https://emacs.stackexchange.com/questions/7148/get-all-regexp-matches-in-buffer-as-a-list
(defun my-matches-in-buffer (regexp &optional buffer)
  "return a list of matches of REGEXP in BUFFER or the current buffer if not given."
  (let ((matches))
    (save-match-data
      (save-excursion
        (with-current-buffer (or buffer (current-buffer))
          (save-restriction
            (widen)
            (goto-char 1)
            (while (search-forward-regexp regexp nil nil 1)
              (push (match-string-no-properties 0) matches)))))
      matches)))

;; https://curiousprogrammer.wordpress.com/2009/05/22/batch-text-processing/
(defun my-make-csv (seq)
  (mapconcat (lambda (e) (format "\"%s\"" e)) seq ", "))

(defun my-make-csv-no-quotes (seq)
  (mapconcat (lambda (e) (format "%s" e)) seq ", "))

;; ;;


(defun my-get-entire-buffer-string ()
  "Gets the current buffer's entire contents and returns it as a string."
  (buffer-substring-no-properties 1 ;; Wow, one-based index..
				  (buffer-size)))

(defun my-regex-replace (orig replacement)
  "Replaces orig with replacement in the current buffer."
  ;;(replace-match replacement nil orig nil (my-get-entire-buffer-string)))
  (save-excursion
    (goto-char (point-min))
    (while (re-search-forward orig nil t)
      (replace-match replacement)
      (forward-line 1))))

(defun my-process-current-grammar-buffer ()
  "This will process the current buffer, which will be treated as being the format of the \"grammar.txt\" file."
  (interactive)
  (my-copy-matches-of-re "[a-z_]+ :") ;; Run on the current buffer.

  ;;(error "temp")
  
  ;; We are now in the *matching* buffer. Move point to the start of the file.
  (beginning-of-buffer)

  (let ((orig "\\([a-z_]+\\) :")
	(replacement "  mpc_parser_t* \\1 = mpc_new(\"\\1\");"))

    ;; Replace everything with what we want:
    (my-regex-replace orig "\\1")

    ;; Copy the whole file to get lines containing each match, in a string:
    (let ((all "\\(^.*$\\)")
	  (entire-buffer (my-get-entire-buffer-string))
	  )
      ;; Trim newline
      (goto-char (point-max))
      (backward-delete-char 1)
      
      ;; https://emacs.stackexchange.com/questions/19518/is-there-an-idiomatic-way-of-reading-each-line-in-a-buffer-to-process-it-line-by :
      ;; (goto-char (point-min))
      ;; (while (not (eobp))
      ;;   (let ((line (buffer-substring (point)
      ;;                                 (progn (forward-line 1) (point)))))
      ;; 	...))


      ;; Create multiple results using the entire-buffer string:
      ;;(erase-buffer) ;; Clear the current buffer

      ;;(my-regex-replace all "mpc_parser_t* \\1_ = mpc_new(\"\\1\");")
      (let ((lst (save-excursion
		   (goto-char (point-min))
		   (cl-loop for line in (split-string entire-buffer "\n")
			    ;; ... process line here ...
			    do (kill-whole-line)
			    for item = (cond ((string-equal line "char") "char_")
					     ((string-equal line "int") "int_")
					     ((string-equal line "float") "float_")
					     ('t line))
			    do (insert (concat "  mpc_parser_t* " item
					       " = mpc_new(\"" line "\");\n"))
			    collect item
			    ;;(ignore-errors (forward-line))
			    ))))

	;;(save-excursion (insert entire-buffer)) ;; Insert new text and restore point to where we were
	;;(save-excursion
	;; Replace stuff here:
	;; Get all matches
	;; (let ((items (my-matches-in-buffer "^.*$"))) ;; Gets a list of all items to make
	;; 	(insert (concat " mpca_lang(MPCA_LANG_DEFAULT,
	;; 	    grammar,
	;; 	   " (my-make-csv items) ", NULL);")))
	;;)

	;; (string-match "^.*$" entire-buffer)
	;; (message (match-string 0))


	;; (dolist (line (split-string entire-buffer "\n")) 
	;; 	;; ... process line here ...
	;; )

	(goto-char (point-max))

	;; (my-make-csv (mapcar (lambda (x) (concat x "_"))
	(insert (concat "\n\n  mpca_lang(MPCA_LANG_DEFAULT,
	    grammar,
	   " ;;(my-make-csv (reverse (split-string entire-buffer "\n")))
			(my-make-csv-no-quotes (reverse lst))
			) ", NULL);;")
	

	;; Save it
	(let ((contents (my-get-entire-buffer-string)))
	  (with-current-buffer (get-file-buffer (file-truename "main.c"))
	    (goto-char (point-min))
	    (search-forward "void run(char* grammar) {\n") ;; Move point to after this
	    (insert contents)
	    (pop-to-buffer (current-buffer))
	    ))

	(let ((contents (my-get-entire-buffer-string)))
	  (with-current-buffer (get-file-buffer (file-truename "main.c"))
	    (goto-char (point-min))
	    (search-forward "}\n\nint main() {") ;; Move point to after this
	    (forward-line -2)
	    (insert (concat "  mpc_cleanup(" (number-to-string (length lst)) ", " (my-make-csv-no-quotes (reverse lst))) ");\n")
	    (pop-to-buffer (current-buffer))
	    ))

	)
      )
    
    ;;(write-region nil nil "
    
    ;;   (query-replace-regexp #("\\([a-z_]+\\) :" 0 6 (face font-lock-comment-face) 6 7 (face font-lock-comment-face) 7 13 (face font-lock-comment-face)) "mpc_parser_t* \\1 = mpc_new(\"\\1\");" nil nil nil nil nil) ;; Replace results in the new buffer.
    )
  
  )
