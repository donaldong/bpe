import torch
from mingpt.utils import set_seed
from mingpt.model import GPT
# create a Trainer object
from mingpt.trainer import Trainer
from TextDataset import TextDataset

set_seed(3407)

train_dataset = TextDataset('./book1.txt', './vocab.bpe', seq_len=20)
model_config = GPT.get_default_config()
model_config.model_type = 'gpt-nano'
model_config.vocab_size = 10833
model_config.block_size = train_dataset.seq_len
model = GPT(model_config)


train_config = Trainer.get_default_config()
train_config.learning_rate = 5e-4 # the model we're using is so small that we can go a bit faster
train_config.max_iters = 3000
train_config.num_workers = 0
trainer = Trainer(train_config, model, train_dataset)

def batch_end_callback(trainer):
    if trainer.iter_num % 100 == 0:
        print(f"iter_dt {trainer.iter_dt * 1000:.2f}ms; iter {trainer.iter_num}: train loss {trainer.loss.item():.5f}")
trainer.set_callback('on_batch_end', batch_end_callback)

trainer.run()
# now let's perform some evaluation
model.eval()


def generate(prompt='', num_samples=10, steps=20, do_sample=True):
    encoder = train_dataset.encoder
    x = torch.tensor([encoder.encode(prompt)], dtype=torch.long)
    x = x.expand(num_samples, -1)
    # forward the model `steps` times to get samples, in a batch
    y = model.generate(x, max_new_tokens=steps, do_sample=do_sample, top_k=40)
    
    for i in range(num_samples):
        out = encoder.decode(y[i].cpu().squeeze().tolist())
        print('-'*80)
        print(out)

generate(prompt='Hello! It is nice to', num_samples=10, steps=20)
