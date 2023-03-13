import torch
from torch.utils.data import Dataset
from BPETokenizer import get_encoder

class TextDataset(Dataset):
  def __init__(self, text_file, vocab_file, seq_len):
    text = open(text_file).read().split('\n')
    self.encoder = get_encoder(vocab_file=vocab_file)
    self.text = []
    self.seq_len = seq_len
    for line in text:
      if len(self.text) > 0:
        self.text += [0]
      self.text += self.encoder.encode(line)

  def __len__(self):
    return len(self.text) - self.seq_len - 1
  
  def __getitem__(self, idx):
    x = [None] * self.seq_len
    y = [None] * self.seq_len
    for i in range(self.seq_len):
      x[i] = self.text[idx + i]
      y[i] = self.text[idx + i + 1]
    return torch.as_tensor(x), torch.as_tensor(y)

if __name__ == '__main__':
  train = TextDataset('./book1.txt', './vocab.bpe')
  print(len(train[0]))
  print(train[0])
