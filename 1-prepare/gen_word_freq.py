import sys
from transformers import AutoTokenizer
from collections import defaultdict

tokenizer = AutoTokenizer.from_pretrained("gpt2")
word_freqs = defaultdict(int)

for text in sys.stdin:
  words_with_offsets = tokenizer.backend_tokenizer.pre_tokenizer.pre_tokenize_str(text)
  new_words = [word for word, offset in words_with_offsets]
  for word in new_words:
    word_freqs[word] += 1

for w, f in word_freqs.items():
  print(w, f)
