function [acc, FP, FN] = classifierAccuracy(model, Y, X)

[Y_hat, acc] = predict(Y, sparse(X), model);

P = (Y > 0);
N = (Y < 0);
FP = nnz((Y_hat>0) & N);
FN = nnz((Y_hat<0) & P);