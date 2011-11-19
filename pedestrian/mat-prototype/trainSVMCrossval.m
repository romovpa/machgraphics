function [modelBest, accuracy] = trainSVMCrossval(X, Y, nfold, C)

if ~exist('C', 'var')
  C = 10.^(-2:6);
end

N = size(X,1);
folds = mod(0:N-1, nfold)+1;

fprintf('Running cross-validation:');

modelBest = [];
accBest = 0;
accuracy = zeros(size(C));
for i = 1:length(C)
  args = sprintf('-c %f -s 1 -q', C(i));
  
  for d = 1:nfold
    objs = (folds~=d);
    valid = (folds==d);
    model = train(Y(objs), sparse(X(objs,:)), args);
    Y_hat = predict(Y(valid), sparse(X(valid,:)), model);
    accuracy(i) = accuracy(i) + nnz(Y_hat==Y(valid));
  end
  
  fprintf('C=%f, accuracy = %d/%d (%f%%)\n', C(i), accuracy(i), ...
    N, accuracy(i)/N*100);
  
  if accBest < accuracy(i)
    accBest = accuracy(i);
    modelBest = model;
  end
end